/*
 * test_board.c — host-side tests for the Board domain C API (whad-lib).
 *
 * BUILD PATTERN: board.c validators are ALL declared `static`, so they are
 * invisible to a separately-compiled translation unit. This file textually
 * includes board.c via `#include "../src/domains/board.c"` to gain direct
 * access to every validator helper. The Makefile MUST NOT list board.c as a
 * separate source file (it would cause duplicate-symbol errors with the
 * X-macro-generated whad_board_* functions).
 *
 * Coverage:
 *   - valid_span boundary/overflow (uint64 promotion protection)
 *   - whad_board_validate_lease (NULL, bad resource, valid)
 *   - whad_board_validate_command_result (NULL, bad enum, NUL-term, valid)
 *   - whad_board_command_result pack/parse round-trip
 *   - whad_board_validate_set_runtime_config (which_operation switch)
 *   - whad_board_validate_audio_chunk (count vs size, span, NULL)
 *   - whad_board_validate_log_chunk (symmetric to audio_chunk)
 *   - whad_board_validate_spi_transfer (cross-struct size, bad mode)
 *   - whad_board_validate_i2c_transfer (cross-struct size)
 *   - whad_board_validate_board_info (4-string NUL + device_id + runtime)
 *   - whad_board_validate_board_status (3 enums + progress_per_mille)
 *   - whad_board_validate_remote_profile_set (mappings_count cap)
 *   - whad_board_validate_storage_read_log (max_bytes cap)
 *   - whad_board_validate_raw_pcm_diagnostics (chunk_size cap)
 *   - whad_board_get_message_type (NULL, non-board, board)
 *   - whad_board_command_from_message_type (28-way switch + default + NULL)
 *   - whad_board_gpio_configure pack/parse round-trip
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/*
 * Textual inclusion — board.c validators are static and thus invisible to a
 * separate TU. Including the .c file directly makes them callable from tests.
 * board.c pulls in <whad.h> and <domains/board.h> via the -I include paths.
 */
#include "../src/domains/board.c"

#define TEST_ASSERT(condition) do { if (!(condition)) { fprintf(stderr, "%s:%d: assertion failed: %s\n", __FILE__, __LINE__, #condition); return false; } } while (0)

/* ------------------------------------------------------------------ */
/* valid_span — used by audio_chunk and log_chunk validators          */
/* ------------------------------------------------------------------ */

static bool test_valid_span_unbounded(void)
{
    /* total=0 means unbounded: any offset/count is valid */
    TEST_ASSERT(whad_board_valid_span(0, 0, 0));
    TEST_ASSERT(whad_board_valid_span(100, 200, 0));
    TEST_ASSERT(whad_board_valid_span(0xFFFFFFFFu, 0xFFFFFFFFu, 0));
    return true;
}

static bool test_valid_span_boundary(void)
{
    /* end == total is the exact boundary (inclusive) */
    TEST_ASSERT(whad_board_valid_span(0, 10, 10));
    TEST_ASSERT(whad_board_valid_span(5, 5, 10));
    TEST_ASSERT(whad_board_valid_span(10, 0, 10));
    /* end > total is invalid */
    TEST_ASSERT(!whad_board_valid_span(8, 5, 10));
    TEST_ASSERT(!whad_board_valid_span(0, 11, 10));
    TEST_ASSERT(!whad_board_valid_span(11, 0, 10));
    return true;
}

static bool test_valid_span_overflow_protection(void)
{
    /*
     * offset=0xFFFFFFFF + count=1 would overflow a uint32_t to 0 without
     * the uint64 promotion in valid_span. The result (0) would be <= any
     * total, wrongly passing. With promotion, end=0x100000000 > total.
     */
    TEST_ASSERT(!whad_board_valid_span(0xFFFFFFFFu, 1, 0xFFFFFFFFu));
    TEST_ASSERT(!whad_board_valid_span(1, 0xFFFFFFFFu, 0xFFFFFFFFu));
    /* Same total but exact fit without overflow is valid */
    TEST_ASSERT(whad_board_valid_span(0xFFFFFFFEu, 1, 0xFFFFFFFFu));
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_board_validate_lease — gates every GPIO/I2C/SPI pin request  */
/* ------------------------------------------------------------------ */

static bool test_validate_lease_null(void)
{
    TEST_ASSERT(whad_board_validate_lease(NULL) == WHAD_ERROR);
    return true;
}

static bool test_validate_lease_bad_resource(void)
{
    board_LeaseToken lease;
    memset(&lease, 0, sizeof(lease));

    /* Resource below enum min */
    lease.resource = (board_ResourceKind)(_board_ResourceKind_MIN - 1);
    TEST_ASSERT(whad_board_validate_lease(&lease) == WHAD_ERROR);

    /* Resource above enum max */
    lease.resource = (board_ResourceKind)(_board_ResourceKind_MAX + 1);
    TEST_ASSERT(whad_board_validate_lease(&lease) == WHAD_ERROR);
    return true;
}

static bool test_validate_lease_valid(void)
{
    board_LeaseToken lease;
    memset(&lease, 0, sizeof(lease));
    lease.resource = board_ResourceKind_RESOURCE_GPIO_PIN;
    lease.instance = 4;
    lease.generation = 1;
    lease.owner = 0;
    TEST_ASSERT(whad_board_validate_lease(&lease) == WHAD_SUCCESS);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_board_validate_command_result                                 */
/* ------------------------------------------------------------------ */

static bool test_validate_command_result_null(void)
{
    TEST_ASSERT(whad_board_validate_command_result(NULL) == WHAD_ERROR);
    return true;
}

static bool test_validate_command_result_bad_enum(void)
{
    board_CommandResult result;
    memset(&result, 0, sizeof(result));

    /* Bad command (below min) */
    result.command = (board_BoardCommand)(_board_BoardCommand_MIN - 1);
    result.result = board_BoardResultCode_SUCCESS;
    result.detail[0] = '\0';
    TEST_ASSERT(whad_board_validate_command_result(&result) == WHAD_ERROR);

    /* Bad result code (above max) */
    result.command = board_BoardCommand_GetBoardInfo;
    result.result = (board_BoardResultCode)(_board_BoardResultCode_MAX + 1);
    TEST_ASSERT(whad_board_validate_command_result(&result) == WHAD_ERROR);
    return true;
}

static bool test_validate_command_result_no_nul(void)
{
    board_CommandResult result;
    memset(&result, 0, sizeof(result));
    result.command = board_BoardCommand_GetBoardInfo;
    result.result = board_BoardResultCode_SUCCESS;

    /* Fill detail with non-NUL bytes — valid_string must reject */
    memset(result.detail, 'A', sizeof(result.detail));
    TEST_ASSERT(whad_board_validate_command_result(&result) == WHAD_ERROR);

    /* A single NUL terminator at position 0 is sufficient */
    result.detail[0] = '\0';
    TEST_ASSERT(whad_board_validate_command_result(&result) == WHAD_SUCCESS);
    return true;
}

static bool test_validate_command_result_valid(void)
{
    board_CommandResult result;
    memset(&result, 0, sizeof(result));
    result.command = board_BoardCommand_AdcRead;
    result.result = board_BoardResultCode_NOT_IMPLEMENTED;
    result.terminal = true;
    strncpy(result.detail, "pin not leased", sizeof(result.detail) - 1);
    result.detail[sizeof(result.detail) - 1] = '\0';
    TEST_ASSERT(whad_board_validate_command_result(&result) == WHAD_SUCCESS);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_board_command_result pack/parse round-trip                   */
/* ------------------------------------------------------------------ */

static bool test_command_result_round_trip(void)
{
    board_CommandResult value;
    Message msg;
    uint32_t request_id;
    board_CommandResult parsed;

    memset(&value, 0, sizeof(value));
    value.command = board_BoardCommand_StorageReadLog;
    value.result = board_BoardResultCode_SUCCESS;
    value.terminal = true;
    strncpy(value.detail, "round-trip", sizeof(value.detail) - 1);
    value.detail[sizeof(value.detail) - 1] = '\0';

    request_id = 0xDEAD;

    /* Pack */
    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_board_command_result(&msg, request_id, &value) == WHAD_SUCCESS);
    TEST_ASSERT(msg.which_msg == Message_board_tag);
    TEST_ASSERT(msg.msg.board.which_msg == board_Message_command_result_tag);
    TEST_ASSERT(msg.msg.board.request_id == request_id);

    /* Unpack */
    memset(&parsed, 0xFF, sizeof(parsed));
    uint32_t parsed_id = 0;
    TEST_ASSERT(whad_board_command_result_parse(&msg, &parsed_id, &parsed) == WHAD_SUCCESS);
    TEST_ASSERT(parsed_id == request_id);
    TEST_ASSERT(parsed.command == value.command);
    TEST_ASSERT(parsed.result == value.result);
    TEST_ASSERT(parsed.terminal == value.terminal);
    TEST_ASSERT(strcmp(parsed.detail, value.detail) == 0);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_board_validate_set_runtime_config — which_operation switch   */
/* ------------------------------------------------------------------ */

static bool test_validate_set_runtime_config(void)
{
    board_SetRuntimeConfigRequest req;
    memset(&req, 0, sizeof(req));

    /* open_pairing_window → SUCCESS */
    req.which_operation = board_SetRuntimeConfigRequest_open_pairing_window_tag;
    TEST_ASSERT(whad_board_validate_set_runtime_config(&req) == WHAD_SUCCESS);

    /* clear_bonds → SUCCESS */
    req.which_operation = board_SetRuntimeConfigRequest_clear_bonds_tag;
    TEST_ASSERT(whad_board_validate_set_runtime_config(&req) == WHAD_SUCCESS);

    /* update with valid persisted_runtime → SUCCESS */
    req.which_operation = board_SetRuntimeConfigRequest_update_tag;
    req.operation.update.has_persisted_runtime = true;
    req.operation.update.persisted_runtime = board_RuntimeMode_RUNTIME_RAW_WHAD;
    TEST_ASSERT(whad_board_validate_set_runtime_config(&req) == WHAD_SUCCESS);

    /* update with invalid persisted_runtime → ERROR */
    req.operation.update.persisted_runtime = (board_RuntimeMode)(_board_RuntimeMode_MAX + 1);
    TEST_ASSERT(whad_board_validate_set_runtime_config(&req) == WHAD_ERROR);

    /* NULL → ERROR */
    TEST_ASSERT(whad_board_validate_set_runtime_config(NULL) == WHAD_ERROR);

    /* Unknown which_operation → ERROR */
    req.which_operation = 99;
    TEST_ASSERT(whad_board_validate_set_runtime_config(&req) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_board_validate_audio_chunk                                    */
/* ------------------------------------------------------------------ */

static bool test_validate_audio_chunk(void)
{
    board_AudioChunk chunk;
    memset(&chunk, 0, sizeof(chunk));

    /* Valid: count <= pcm.size, span within total */
    chunk.result = board_BoardResultCode_SUCCESS;
    chunk.offset = 0;
    chunk.count = 10;
    chunk.total = 100;
    chunk.pcm.size = 10;
    TEST_ASSERT(whad_board_validate_audio_chunk(&chunk) == WHAD_SUCCESS);

    /* count > pcm.size → ERROR */
    chunk.count = 20;
    chunk.pcm.size = 10;
    TEST_ASSERT(whad_board_validate_audio_chunk(&chunk) == WHAD_ERROR);

    /* span overflow → ERROR */
    chunk.count = 10;
    chunk.pcm.size = 10;
    chunk.offset = 95;
    chunk.total = 100;
    TEST_ASSERT(whad_board_validate_audio_chunk(&chunk) == WHAD_ERROR);

    /* unbounded total → span always valid */
    chunk.total = 0;
    chunk.offset = 0xFFFFFFFFu;
    chunk.count = 10;
    chunk.pcm.size = 10;
    TEST_ASSERT(whad_board_validate_audio_chunk(&chunk) == WHAD_SUCCESS);

    /* bad result enum → ERROR */
    chunk.result = (board_BoardResultCode)(_board_BoardResultCode_MAX + 1);
    chunk.offset = 0;
    chunk.total = 100;
    TEST_ASSERT(whad_board_validate_audio_chunk(&chunk) == WHAD_ERROR);

    /* NULL → ERROR */
    TEST_ASSERT(whad_board_validate_audio_chunk(NULL) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_board_validate_log_chunk — symmetric to audio_chunk          */
/* ------------------------------------------------------------------ */

static bool test_validate_log_chunk(void)
{
    board_LogChunk chunk;
    memset(&chunk, 0, sizeof(chunk));

    /* Valid */
    chunk.result = board_BoardResultCode_SUCCESS;
    chunk.offset = 0;
    chunk.count = 50;
    chunk.total = 200;
    chunk.data.size = 50;
    TEST_ASSERT(whad_board_validate_log_chunk(&chunk) == WHAD_SUCCESS);

    /* count > data.size → ERROR */
    chunk.count = 100;
    chunk.data.size = 50;
    TEST_ASSERT(whad_board_validate_log_chunk(&chunk) == WHAD_ERROR);

    /* span overflow → ERROR */
    chunk.count = 50;
    chunk.data.size = 50;
    chunk.offset = 190;
    chunk.total = 200;
    TEST_ASSERT(whad_board_validate_log_chunk(&chunk) == WHAD_ERROR);

    /* bad result → ERROR */
    chunk.result = (board_BoardResultCode)(_board_BoardResultCode_MAX + 1);
    chunk.offset = 0;
    chunk.total = 0;
    TEST_ASSERT(whad_board_validate_log_chunk(&chunk) == WHAD_ERROR);

    /* NULL → ERROR */
    TEST_ASSERT(whad_board_validate_log_chunk(NULL) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_board_validate_spi_transfer — cross-struct size trick        */
/* ------------------------------------------------------------------ */

static bool test_validate_spi_transfer(void)
{
    board_SpiTransferRequest req;
    memset(&req, 0, sizeof(req));

    /* Valid */
    req.mode = board_SpiMode_SPI_MODE_0;
    req.tx_data.size = 4;
    req.read_length = 64;
    req.has_lease = false;
    TEST_ASSERT(whad_board_validate_spi_transfer(&req) == WHAD_SUCCESS);

    /* Bad SPI mode → ERROR */
    req.mode = (board_SpiMode)(_board_SpiMode_MAX + 1);
    TEST_ASSERT(whad_board_validate_spi_transfer(&req) == WHAD_ERROR);

    /* read_length exceeds response rx_data capacity (512 bytes) → ERROR */
    req.mode = board_SpiMode_SPI_MODE_0;
    req.read_length = 513;
    TEST_ASSERT(whad_board_validate_spi_transfer(&req) == WHAD_ERROR);

    /* read_length at boundary (512) → SUCCESS */
    req.read_length = 512;
    TEST_ASSERT(whad_board_validate_spi_transfer(&req) == WHAD_SUCCESS);

    /* NULL → ERROR */
    TEST_ASSERT(whad_board_validate_spi_transfer(NULL) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_board_validate_i2c_transfer — cross-struct size trick        */
/* ------------------------------------------------------------------ */

static bool test_validate_i2c_transfer(void)
{
    board_I2cTransferRequest req;
    memset(&req, 0, sizeof(req));

    /* Valid */
    req.address = 0x50;
    req.write_data.size = 2;
    req.read_length = 32;
    req.has_lease = false;
    TEST_ASSERT(whad_board_validate_i2c_transfer(&req) == WHAD_SUCCESS);

    /* read_length exceeds response read_data capacity (512) → ERROR */
    req.read_length = 513;
    TEST_ASSERT(whad_board_validate_i2c_transfer(&req) == WHAD_ERROR);

    /* Boundary (512) → SUCCESS */
    req.read_length = 512;
    TEST_ASSERT(whad_board_validate_i2c_transfer(&req) == WHAD_SUCCESS);

    /* With lease */
    req.has_lease = true;
    req.lease.resource = board_ResourceKind_RESOURCE_I2C_BUS;
    req.lease.instance = 0;
    TEST_ASSERT(whad_board_validate_i2c_transfer(&req) == WHAD_SUCCESS);

    /* With bad lease → ERROR */
    req.lease.resource = (board_ResourceKind)(_board_ResourceKind_MAX + 1);
    TEST_ASSERT(whad_board_validate_i2c_transfer(&req) == WHAD_ERROR);

    /* NULL → ERROR */
    TEST_ASSERT(whad_board_validate_i2c_transfer(NULL) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_board_validate_board_info — 4 strings + device_id + runtime  */
/* ------------------------------------------------------------------ */

static bool test_validate_board_info(void)
{
    board_GetBoardInfoResponse resp;
    memset(&resp, 0, sizeof(resp));

    /* All valid */
    strncpy(resp.board_name, "Adafruit CLUE", sizeof(resp.board_name) - 1);
    strncpy(resp.hardware_revision, "nRF52840", sizeof(resp.hardware_revision) - 1);
    strncpy(resp.firmware_version, "v1.2.0", sizeof(resp.firmware_version) - 1);
    strncpy(resp.protocol_variant, "whad-clue", sizeof(resp.protocol_variant) - 1);
    resp.active_runtime = board_RuntimeMode_RUNTIME_RAW_WHAD;
    resp.device_id.size = 4;
    TEST_ASSERT(whad_board_validate_board_info(&resp) == WHAD_SUCCESS);

    /* board_name without NUL → ERROR */
    memset(resp.board_name, 'B', sizeof(resp.board_name));
    TEST_ASSERT(whad_board_validate_board_info(&resp) == WHAD_ERROR);
    resp.board_name[0] = '\0';

    /* firmware_version without NUL → ERROR */
    memset(resp.firmware_version, 'F', sizeof(resp.firmware_version));
    TEST_ASSERT(whad_board_validate_board_info(&resp) == WHAD_ERROR);
    resp.firmware_version[0] = '\0';

    /* device_id.size exceeds capacity (16) → ERROR */
    resp.device_id.size = 17;
    TEST_ASSERT(whad_board_validate_board_info(&resp) == WHAD_ERROR);
    resp.device_id.size = 4;

    /* bad active_runtime → ERROR */
    resp.active_runtime = (board_RuntimeMode)(_board_RuntimeMode_MAX + 1);
    TEST_ASSERT(whad_board_validate_board_info(&resp) == WHAD_ERROR);

    /* NULL → ERROR */
    TEST_ASSERT(whad_board_validate_board_info(NULL) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_board_validate_board_status — 3 enums + progress_per_mille   */
/* ------------------------------------------------------------------ */

static bool test_validate_board_status(void)
{
    board_BoardStatus status;
    memset(&status, 0, sizeof(status));

    /* All valid */
    status.code = board_BoardStatusCode_BOARD_STATUS_STORAGE_PROGRESS;
    status.result = board_BoardResultCode_SUCCESS;
    status.resource = board_ResourceKind_RESOURCE_STORAGE;
    status.progress_per_mille = 500;
    status.detail[0] = '\0';
    TEST_ASSERT(whad_board_validate_board_status(&status) == WHAD_SUCCESS);

    /* progress_per_mille boundary: 1000 is valid */
    status.progress_per_mille = 1000;
    TEST_ASSERT(whad_board_validate_board_status(&status) == WHAD_SUCCESS);

    /* progress_per_mille over 1000 → ERROR */
    status.progress_per_mille = 1001;
    TEST_ASSERT(whad_board_validate_board_status(&status) == WHAD_ERROR);
    status.progress_per_mille = 500;

    /* bad code enum → ERROR */
    status.code = (board_BoardStatusCode)(_board_BoardStatusCode_MAX + 1);
    TEST_ASSERT(whad_board_validate_board_status(&status) == WHAD_ERROR);
    status.code = board_BoardStatusCode_BOARD_STATUS_UNKNOWN;

    /* bad result enum → ERROR */
    status.result = (board_BoardResultCode)(_board_BoardResultCode_MAX + 1);
    TEST_ASSERT(whad_board_validate_board_status(&status) == WHAD_ERROR);

    /* NULL → ERROR */
    TEST_ASSERT(whad_board_validate_board_status(NULL) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_board_validate_remote_profile_set — mappings_count cap       */
/* ------------------------------------------------------------------ */

static bool test_validate_remote_profile_set(void)
{
    board_RemoteProfileSetRequest req;
    memset(&req, 0, sizeof(req));

    /* No profile → SUCCESS */
    req.has_profile = false;
    TEST_ASSERT(whad_board_validate_remote_profile_set(&req) == WHAD_SUCCESS);

    /* Valid profile with mappings_count = 0 → SUCCESS */
    req.has_profile = true;
    req.profile.profile_id = 1;
    req.profile.name[0] = '\0';
    req.profile.mappings_count = 0;
    TEST_ASSERT(whad_board_validate_remote_profile_set(&req) == WHAD_SUCCESS);

    /* mappings_count at cap (16) → SUCCESS */
    req.profile.mappings_count = 16;
    {
        size_t i;
        for (i = 0; i < 16; i++)
        {
            req.profile.mappings[i].source = board_InputSource_INPUT_SOURCE_BUTTON_A;
            req.profile.mappings[i].action = board_InputAction_INPUT_ACTION_PRESS;
        }
    }
    TEST_ASSERT(whad_board_validate_remote_profile_set(&req) == WHAD_SUCCESS);

    /* mappings_count exceeds cap (17) → ERROR */
    req.profile.mappings_count = 17;
    TEST_ASSERT(whad_board_validate_remote_profile_set(&req) == WHAD_ERROR);

    /* name without NUL → ERROR */
    req.profile.mappings_count = 0;
    memset(req.profile.name, 'X', sizeof(req.profile.name));
    TEST_ASSERT(whad_board_validate_remote_profile_set(&req) == WHAD_ERROR);

    /* NULL → ERROR */
    TEST_ASSERT(whad_board_validate_remote_profile_set(NULL) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_board_validate_storage_read_log — max_bytes cap              */
/* ------------------------------------------------------------------ */

static bool test_validate_storage_read_log(void)
{
    board_StorageReadLogRequest req;
    memset(&req, 0, sizeof(req));

    /* Valid: max_bytes within LogChunk.data capacity (900 bytes) */
    req.max_bytes = 256;
    TEST_ASSERT(whad_board_validate_storage_read_log(&req) == WHAD_SUCCESS);

    /* Boundary: max_bytes = 900 (exact capacity) → SUCCESS */
    req.max_bytes = 900;
    TEST_ASSERT(whad_board_validate_storage_read_log(&req) == WHAD_SUCCESS);

    /* Over capacity: max_bytes = 901 → ERROR */
    req.max_bytes = 901;
    TEST_ASSERT(whad_board_validate_storage_read_log(&req) == WHAD_ERROR);

    /* NULL → ERROR */
    TEST_ASSERT(whad_board_validate_storage_read_log(NULL) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_board_validate_raw_pcm_diagnostics — chunk_size cap          */
/* ------------------------------------------------------------------ */

static bool test_validate_raw_pcm_diagnostics(void)
{
    board_RawPcmDiagnosticsRequest req;
    memset(&req, 0, sizeof(req));

    /* Valid: chunk_size within AudioChunk.pcm capacity (800 bytes) */
    req.chunk_size = 256;
    req.duration_ms = 1000;
    TEST_ASSERT(whad_board_validate_raw_pcm_diagnostics(&req) == WHAD_SUCCESS);

    /* Boundary: chunk_size = 800 → SUCCESS */
    req.chunk_size = 800;
    TEST_ASSERT(whad_board_validate_raw_pcm_diagnostics(&req) == WHAD_SUCCESS);

    /* Over capacity: chunk_size = 801 → ERROR */
    req.chunk_size = 801;
    TEST_ASSERT(whad_board_validate_raw_pcm_diagnostics(&req) == WHAD_ERROR);

    /* NULL → ERROR */
    TEST_ASSERT(whad_board_validate_raw_pcm_diagnostics(NULL) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_board_get_message_type                                        */
/* ------------------------------------------------------------------ */

static bool test_get_message_type(void)
{
    Message msg;
    memset(&msg, 0, sizeof(msg));

    /* NULL → UNKNOWN */
    TEST_ASSERT(whad_board_get_message_type(NULL) == WHAD_BOARD_UNKNOWN);

    /* Non-board domain → UNKNOWN */
    msg.which_msg = Message_generic_tag;
    TEST_ASSERT(whad_board_get_message_type(&msg) == WHAD_BOARD_UNKNOWN);

    /* Board domain with command_result tag */
    msg.which_msg = Message_board_tag;
    msg.msg.board.which_msg = board_Message_command_result_tag;
    TEST_ASSERT(whad_board_get_message_type(&msg) == WHAD_BOARD_COMMAND_RESULT);

    /* Board domain with gpio_configure tag */
    msg.msg.board.which_msg = board_Message_gpio_configure_tag;
    TEST_ASSERT(whad_board_get_message_type(&msg) == WHAD_BOARD_GPIO_CONFIGURE);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_board_command_from_message_type — 28-way switch + default    */
/* ------------------------------------------------------------------ */

static bool test_command_from_message_type(void)
{
    board_BoardCommand cmd;

    /* NULL p_command → ERROR */
    TEST_ASSERT(whad_board_command_from_message_type(WHAD_BOARD_GET_BOARD_INFO, NULL) == WHAD_ERROR);

    /* First command */
    TEST_ASSERT(whad_board_command_from_message_type(WHAD_BOARD_GET_BOARD_INFO, &cmd) == WHAD_SUCCESS);
    TEST_ASSERT(cmd == board_BoardCommand_GetBoardInfo);

    /* Last command (28th) */
    TEST_ASSERT(whad_board_command_from_message_type(WHAD_BOARD_RAW_PCM_DIAGNOSTICS, &cmd) == WHAD_SUCCESS);
    TEST_ASSERT(cmd == board_BoardCommand_RawPcmDiagnostics);

    /* Mid-range: GPIO_CONFIGURE */
    TEST_ASSERT(whad_board_command_from_message_type(WHAD_BOARD_GPIO_CONFIGURE, &cmd) == WHAD_SUCCESS);
    TEST_ASSERT(cmd == board_BoardCommand_GpioConfigure);

    /* Mid-range: STORAGE_READ_LOG */
    TEST_ASSERT(whad_board_command_from_message_type(WHAD_BOARD_STORAGE_READ_LOG, &cmd) == WHAD_SUCCESS);
    TEST_ASSERT(cmd == board_BoardCommand_StorageReadLog);

    /* Response tag (not a command) → ERROR (falls to default) */
    TEST_ASSERT(whad_board_command_from_message_type(WHAD_BOARD_COMMAND_RESULT, &cmd) == WHAD_ERROR);

    /* UNKNOWN → ERROR */
    TEST_ASSERT(whad_board_command_from_message_type(WHAD_BOARD_UNKNOWN, &cmd) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_board_gpio_configure pack/parse round-trip                   */
/* ------------------------------------------------------------------ */

static bool test_gpio_configure_round_trip(void)
{
    board_GpioConfigureRequest value;
    Message msg;
    uint32_t parsed_id;
    board_GpioConfigureRequest parsed;

    memset(&value, 0, sizeof(value));
    value.pin = 4;
    value.direction = board_GpioDirection_GPIO_OUTPUT;
    value.pull = board_GpioPull_GPIO_PULL_NONE;
    value.initial_value = true;
    value.force = false;
    value.has_lease = true;
    value.lease.resource = board_ResourceKind_RESOURCE_GPIO_PIN;
    value.lease.instance = 4;
    value.lease.generation = 2;
    value.lease.owner = 0;

    uint32_t request_id = 0xBEEF;

    /* Pack */
    memset(&msg, 0, sizeof(msg));
    TEST_ASSERT(whad_board_gpio_configure(&msg, request_id, &value) == WHAD_SUCCESS);
    TEST_ASSERT(msg.which_msg == Message_board_tag);
    TEST_ASSERT(msg.msg.board.which_msg == board_Message_gpio_configure_tag);
    TEST_ASSERT(msg.msg.board.request_id == request_id);

    /* Unpack */
    memset(&parsed, 0, sizeof(parsed));
    parsed_id = 0;
    TEST_ASSERT(whad_board_gpio_configure_parse(&msg, &parsed_id, &parsed) == WHAD_SUCCESS);
    TEST_ASSERT(parsed_id == request_id);
    TEST_ASSERT(parsed.pin == value.pin);
    TEST_ASSERT(parsed.direction == value.direction);
    TEST_ASSERT(parsed.pull == value.pull);
    TEST_ASSERT(parsed.initial_value == value.initial_value);
    TEST_ASSERT(parsed.has_lease == value.has_lease);
    TEST_ASSERT(parsed.lease.resource == value.lease.resource);
    TEST_ASSERT(parsed.lease.generation == value.lease.generation);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_board_validate_storage_status                                 */
/* ------------------------------------------------------------------ */

static bool test_validate_storage_status(void)
{
    board_StorageInfoResponse resp;
    memset(&resp, 0, sizeof(resp));

    /* Valid state */
    resp.state = board_StorageState_STORAGE_ADOPTED;
    TEST_ASSERT(whad_board_validate_storage_status(&resp) == WHAD_SUCCESS);

    /* Boundary min */
    resp.state = (board_StorageState)_board_StorageState_MIN;
    TEST_ASSERT(whad_board_validate_storage_status(&resp) == WHAD_SUCCESS);

    /* Out of range */
    resp.state = (board_StorageState)(_board_StorageState_MAX + 1);
    TEST_ASSERT(whad_board_validate_storage_status(&resp) == WHAD_ERROR);

    /* NULL */
    TEST_ASSERT(whad_board_validate_storage_status(NULL) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_board_validate_set_runtime_mode                               */
/* ------------------------------------------------------------------ */

static bool test_validate_set_runtime_mode(void)
{
    board_SetRuntimeModeRequest req;
    memset(&req, 0, sizeof(req));

    /* Valid mode */
    req.runtime = board_RuntimeMode_RUNTIME_BLE_HID;
    TEST_ASSERT(whad_board_validate_set_runtime_mode(&req) == WHAD_SUCCESS);

    /* Out of range */
    req.runtime = (board_RuntimeMode)(_board_RuntimeMode_MAX + 1);
    TEST_ASSERT(whad_board_validate_set_runtime_mode(&req) == WHAD_ERROR);

    /* NULL */
    TEST_ASSERT(whad_board_validate_set_runtime_mode(NULL) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_board_validate_set_output — target enum + optional lease     */
/* ------------------------------------------------------------------ */

static bool test_validate_set_output(void)
{
    board_SetOutputRequest req;
    memset(&req, 0, sizeof(req));

    /* Valid: known target, no lease */
    req.target = board_OutputTarget_OUTPUT_BUZZER;
    req.value = 1000;
    req.duration_ms = 200;
    TEST_ASSERT(whad_board_validate_set_output(&req) == WHAD_SUCCESS);

    /* Boundary: MIN/MAX targets valid */
    req.target = (board_OutputTarget)_board_OutputTarget_MIN;
    TEST_ASSERT(whad_board_validate_set_output(&req) == WHAD_SUCCESS);
    req.target = (board_OutputTarget)_board_OutputTarget_MAX;
    TEST_ASSERT(whad_board_validate_set_output(&req) == WHAD_SUCCESS);

    /* Bad target enum → ERROR */
    req.target = (board_OutputTarget)(_board_OutputTarget_MAX + 1);
    TEST_ASSERT(whad_board_validate_set_output(&req) == WHAD_ERROR);
    req.target = (board_OutputTarget)(_board_OutputTarget_MIN - 1);
    TEST_ASSERT(whad_board_validate_set_output(&req) == WHAD_ERROR);

    /* With valid lease → SUCCESS */
    req.target = board_OutputTarget_OUTPUT_BUZZER;
    req.has_lease = true;
    req.lease.resource = board_ResourceKind_RESOURCE_GPIO_PIN;
    req.lease.instance = 4;
    req.lease.generation = 1;
    TEST_ASSERT(whad_board_validate_set_output(&req) == WHAD_SUCCESS);

    /* With bad lease → ERROR */
    req.lease.resource = (board_ResourceKind)(_board_ResourceKind_MAX + 1);
    TEST_ASSERT(whad_board_validate_set_output(&req) == WHAD_ERROR);

    /* NULL → ERROR */
    TEST_ASSERT(whad_board_validate_set_output(NULL) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_board_validate_configure_input — mode enum                   */
/* ------------------------------------------------------------------ */

static bool test_validate_configure_input(void)
{
    board_ConfigureInputRequest req;
    memset(&req, 0, sizeof(req));

    /* Valid mode */
    req.mode = board_InputMode_INPUT_MODE_REMOTE;
    TEST_ASSERT(whad_board_validate_configure_input(&req) == WHAD_SUCCESS);

    /* Boundary MIN/MAX */
    req.mode = (board_InputMode)_board_InputMode_MIN;
    TEST_ASSERT(whad_board_validate_configure_input(&req) == WHAD_SUCCESS);
    req.mode = (board_InputMode)_board_InputMode_MAX;
    TEST_ASSERT(whad_board_validate_configure_input(&req) == WHAD_SUCCESS);

    /* Out of range */
    req.mode = (board_InputMode)(_board_InputMode_MAX + 1);
    TEST_ASSERT(whad_board_validate_configure_input(&req) == WHAD_ERROR);
    req.mode = (board_InputMode)(_board_InputMode_MIN - 1);
    TEST_ASSERT(whad_board_validate_configure_input(&req) == WHAD_ERROR);

    /* NULL */
    TEST_ASSERT(whad_board_validate_configure_input(NULL) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_board_validate_gpio_configure — bounds (direction + pull)    */
/* ------------------------------------------------------------------ */

static bool test_validate_gpio_configure_bounds(void)
{
    board_GpioConfigureRequest req;
    memset(&req, 0, sizeof(req));

    /* Valid */
    req.pin = 4;
    req.direction = board_GpioDirection_GPIO_OUTPUT;
    req.pull = board_GpioPull_GPIO_PULL_NONE;
    TEST_ASSERT(whad_board_validate_gpio_configure(&req) == WHAD_SUCCESS);

    /* Boundary direction MIN/MAX */
    req.direction = (board_GpioDirection)_board_GpioDirection_MIN;
    req.pull = board_GpioPull_GPIO_PULL_NONE;
    TEST_ASSERT(whad_board_validate_gpio_configure(&req) == WHAD_SUCCESS);
    req.direction = (board_GpioDirection)_board_GpioDirection_MAX;
    TEST_ASSERT(whad_board_validate_gpio_configure(&req) == WHAD_SUCCESS);

    /* Bad direction → ERROR */
    req.direction = (board_GpioDirection)(_board_GpioDirection_MAX + 1);
    TEST_ASSERT(whad_board_validate_gpio_configure(&req) == WHAD_ERROR);

    /* Bad pull → ERROR */
    req.direction = board_GpioDirection_GPIO_OUTPUT;
    req.pull = (board_GpioPull)(_board_GpioPull_MAX + 1);
    TEST_ASSERT(whad_board_validate_gpio_configure(&req) == WHAD_ERROR);

    /* With valid lease → SUCCESS */
    req.pull = board_GpioPull_GPIO_PULL_NONE;
    req.has_lease = true;
    req.lease.resource = board_ResourceKind_RESOURCE_GPIO_PIN;
    req.lease.instance = 4;
    TEST_ASSERT(whad_board_validate_gpio_configure(&req) == WHAD_SUCCESS);

    /* With bad lease → ERROR */
    req.lease.resource = (board_ResourceKind)(_board_ResourceKind_MAX + 1);
    TEST_ASSERT(whad_board_validate_gpio_configure(&req) == WHAD_ERROR);

    /* NULL */
    TEST_ASSERT(whad_board_validate_gpio_configure(NULL) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_board_validate_gpio_read — optional lease                    */
/* ------------------------------------------------------------------ */

static bool test_validate_gpio_read(void)
{
    board_GpioReadRequest req;
    memset(&req, 0, sizeof(req));

    /* Valid: no lease */
    req.pin = 4;
    TEST_ASSERT(whad_board_validate_gpio_read(&req) == WHAD_SUCCESS);

    /* With valid lease → SUCCESS */
    req.has_lease = true;
    req.lease.resource = board_ResourceKind_RESOURCE_GPIO_PIN;
    req.lease.instance = 4;
    TEST_ASSERT(whad_board_validate_gpio_read(&req) == WHAD_SUCCESS);

    /* Bad lease → ERROR */
    req.lease.resource = (board_ResourceKind)(_board_ResourceKind_MAX + 1);
    TEST_ASSERT(whad_board_validate_gpio_read(&req) == WHAD_ERROR);

    /* NULL */
    TEST_ASSERT(whad_board_validate_gpio_read(NULL) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_board_validate_gpio_write — optional lease                   */
/* ------------------------------------------------------------------ */

static bool test_validate_gpio_write(void)
{
    board_GpioWriteRequest req;
    memset(&req, 0, sizeof(req));

    /* Valid: no lease */
    req.pin = 4;
    req.value = true;
    TEST_ASSERT(whad_board_validate_gpio_write(&req) == WHAD_SUCCESS);

    /* With valid lease → SUCCESS */
    req.has_lease = true;
    req.lease.resource = board_ResourceKind_RESOURCE_GPIO_PIN;
    req.lease.instance = 4;
    TEST_ASSERT(whad_board_validate_gpio_write(&req) == WHAD_SUCCESS);

    /* Bad lease → ERROR */
    req.lease.resource = (board_ResourceKind)(_board_ResourceKind_MAX + 1);
    TEST_ASSERT(whad_board_validate_gpio_write(&req) == WHAD_ERROR);

    /* NULL */
    TEST_ASSERT(whad_board_validate_gpio_write(NULL) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_board_validate_release_pin — optional lease + resource enum  */
/* ------------------------------------------------------------------ */

static bool test_validate_release_pin(void)
{
    board_ReleasePinRequest req;
    memset(&req, 0, sizeof(req));

    /* Valid: no lease, valid resource */
    req.resource = board_ResourceKind_RESOURCE_GPIO_PIN;
    req.instance = 4;
    TEST_ASSERT(whad_board_validate_release_pin(&req) == WHAD_SUCCESS);

    /* Boundary MIN/MAX resource */
    req.resource = (board_ResourceKind)_board_ResourceKind_MIN;
    TEST_ASSERT(whad_board_validate_release_pin(&req) == WHAD_SUCCESS);
    req.resource = (board_ResourceKind)_board_ResourceKind_MAX;
    TEST_ASSERT(whad_board_validate_release_pin(&req) == WHAD_SUCCESS);

    /* Bad resource → ERROR */
    req.resource = (board_ResourceKind)(_board_ResourceKind_MAX + 1);
    TEST_ASSERT(whad_board_validate_release_pin(&req) == WHAD_ERROR);

    /* Valid resource + valid lease → SUCCESS */
    req.resource = board_ResourceKind_RESOURCE_GPIO_PIN;
    req.has_lease = true;
    req.lease.resource = board_ResourceKind_RESOURCE_GPIO_PIN;
    req.lease.instance = 4;
    TEST_ASSERT(whad_board_validate_release_pin(&req) == WHAD_SUCCESS);

    /* Valid resource + bad lease → ERROR */
    req.lease.resource = (board_ResourceKind)(_board_ResourceKind_MAX + 1);
    TEST_ASSERT(whad_board_validate_release_pin(&req) == WHAD_ERROR);

    /* NULL */
    TEST_ASSERT(whad_board_validate_release_pin(NULL) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_board_validate_sensor_descriptor — nested descriptor         */
/* ------------------------------------------------------------------ */

static bool test_validate_sensor_descriptor(void)
{
    board_ListSensorsResponse resp;
    memset(&resp, 0, sizeof(resp));

    /* No descriptor → SUCCESS */
    resp.has_descriptor = false;
    TEST_ASSERT(whad_board_validate_sensor_descriptor(&resp) == WHAD_SUCCESS);

    /* Valid descriptor → SUCCESS */
    resp.has_descriptor = true;
    resp.descriptor.sensor_id = 1;
    strncpy(resp.descriptor.name, "acceleration", sizeof(resp.descriptor.name) - 1);
    strncpy(resp.descriptor.unit, "mg", sizeof(resp.descriptor.unit) - 1);
    resp.descriptor.value_count = 3;
    resp.descriptor.component_names_count = 0;
    resp.descriptor.supported_rates_millihz_count = 0;
    TEST_ASSERT(whad_board_validate_sensor_descriptor(&resp) == WHAD_SUCCESS);

    /* name without NUL → ERROR */
    memset(resp.descriptor.name, 'N', sizeof(resp.descriptor.name));
    TEST_ASSERT(whad_board_validate_sensor_descriptor(&resp) == WHAD_ERROR);
    resp.descriptor.name[0] = '\0';

    /* unit without NUL → ERROR */
    memset(resp.descriptor.unit, 'U', sizeof(resp.descriptor.unit));
    TEST_ASSERT(whad_board_validate_sensor_descriptor(&resp) == WHAD_ERROR);
    resp.descriptor.unit[0] = '\0';

    /* component_names_count over cap (11 > 10) → ERROR */
    resp.descriptor.component_names_count = 11;
    TEST_ASSERT(whad_board_validate_sensor_descriptor(&resp) == WHAD_ERROR);

    /* component_names_count at cap (10) with valid strings → SUCCESS */
    resp.descriptor.component_names_count = 10;
    {
        size_t i;
        for (i = 0; i < 10; i++)
        {
            resp.descriptor.component_names[i][0] = '\0';
        }
    }
    TEST_ASSERT(whad_board_validate_sensor_descriptor(&resp) == WHAD_SUCCESS);

    /* component_names with missing NUL → ERROR */
    memset(resp.descriptor.component_names[0], 'C', sizeof(resp.descriptor.component_names[0]));
    TEST_ASSERT(whad_board_validate_sensor_descriptor(&resp) == WHAD_ERROR);
    resp.descriptor.component_names[0][0] = '\0';

    /* supported_rates_millihz_count over cap (9 > 8) → ERROR */
    resp.descriptor.component_names_count = 0;
    resp.descriptor.supported_rates_millihz_count = 9;
    TEST_ASSERT(whad_board_validate_sensor_descriptor(&resp) == WHAD_ERROR);

    /* NULL */
    TEST_ASSERT(whad_board_validate_sensor_descriptor(NULL) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_board_validate_sensor_sample — count cap + status bitmask    */
/* ------------------------------------------------------------------ */

static bool test_validate_sensor_sample(void)
{
    board_SensorSample sample;
    memset(&sample, 0, sizeof(sample));

    /* Valid: empty values, status=NONE */
    sample.sensor_id = 1;
    sample.values_count = 0;
    sample.status = board_SensorStatusFlag_SENSOR_STATUS_NONE;
    TEST_ASSERT(whad_board_validate_sensor_sample(&sample) == WHAD_SUCCESS);

    /* Valid: max values_count (10) */
    sample.values_count = 10;
    TEST_ASSERT(whad_board_validate_sensor_sample(&sample) == WHAD_SUCCESS);

    /* values_count over cap (11 > 10) → ERROR */
    sample.values_count = 11;
    TEST_ASSERT(whad_board_validate_sensor_sample(&sample) == WHAD_ERROR);

    /* All defined status flags → SUCCESS */
    sample.values_count = 0;
    sample.status = (uint32_t)board_SensorStatusFlag_SENSOR_STATUS_CALIBRATED
                  | (uint32_t)board_SensorStatusFlag_SENSOR_STATUS_CALIBRATING
                  | (uint32_t)board_SensorStatusFlag_SENSOR_STATUS_STALE
                  | (uint32_t)board_SensorStatusFlag_SENSOR_STATUS_SATURATED
                  | (uint32_t)board_SensorStatusFlag_SENSOR_STATUS_OVERRUN
                  | (uint32_t)board_SensorStatusFlag_SENSOR_STATUS_MAGNETIC_DISTURBANCE
                  | (uint32_t)board_SensorStatusFlag_SENSOR_STATUS_SENSOR_FAULT;
    TEST_ASSERT(whad_board_validate_sensor_sample(&sample) == WHAD_SUCCESS);

    /* Undefined status bit (0x80) → ERROR */
    sample.status = 0x80u;
    TEST_ASSERT(whad_board_validate_sensor_sample(&sample) == WHAD_ERROR);

    /* NULL */
    TEST_ASSERT(whad_board_validate_sensor_sample(NULL) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_board_validate_calibration — calibration_data.size cap       */
/* ------------------------------------------------------------------ */

static bool test_validate_calibration(void)
{
    board_CalibrationResponse resp;
    memset(&resp, 0, sizeof(resp));

    /* Valid: size within capacity */
    resp.sensor_id = 1;
    resp.calibration_data.size = 16;
    TEST_ASSERT(whad_board_validate_calibration(&resp) == WHAD_SUCCESS);

    /* Boundary: size = sizeof(bytes) (128) → SUCCESS */
    resp.calibration_data.size = sizeof(resp.calibration_data.bytes);
    TEST_ASSERT(whad_board_validate_calibration(&resp) == WHAD_SUCCESS);

    /* size over capacity → ERROR */
    resp.calibration_data.size = (pb_size_t)(sizeof(resp.calibration_data.bytes) + 1);
    TEST_ASSERT(whad_board_validate_calibration(&resp) == WHAD_ERROR);

    /* NULL */
    TEST_ASSERT(whad_board_validate_calibration(NULL) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_board_validate_input_state — gesture + mode enums            */
/* ------------------------------------------------------------------ */

static bool test_validate_input_state(void)
{
    board_InputStateResponse resp;
    memset(&resp, 0, sizeof(resp));

    /* Valid */
    resp.gesture = board_Gesture_GESTURE_UP;
    resp.mode = board_InputMode_INPUT_MODE_REMOTE;
    TEST_ASSERT(whad_board_validate_input_state(&resp) == WHAD_SUCCESS);

    /* Boundary gesture MIN/MAX */
    resp.gesture = (board_Gesture)_board_Gesture_MIN;
    TEST_ASSERT(whad_board_validate_input_state(&resp) == WHAD_SUCCESS);
    resp.gesture = (board_Gesture)_board_Gesture_MAX;
    TEST_ASSERT(whad_board_validate_input_state(&resp) == WHAD_SUCCESS);

    /* Bad gesture → ERROR */
    resp.gesture = (board_Gesture)(_board_Gesture_MAX + 1);
    TEST_ASSERT(whad_board_validate_input_state(&resp) == WHAD_ERROR);

    /* Bad mode → ERROR */
    resp.gesture = board_Gesture_GESTURE_UP;
    resp.mode = (board_InputMode)(_board_InputMode_MAX + 1);
    TEST_ASSERT(whad_board_validate_input_state(&resp) == WHAD_ERROR);

    /* NULL */
    TEST_ASSERT(whad_board_validate_input_state(NULL) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_board_validate_i2c_result — result + size + bytes_read + lse */
/* ------------------------------------------------------------------ */

static bool test_validate_i2c_result(void)
{
    board_I2cTransferResponse resp;
    memset(&resp, 0, sizeof(resp));

    /* Valid */
    resp.result = board_BoardResultCode_SUCCESS;
    resp.read_data.size = 4;
    resp.bytes_read = 4;
    TEST_ASSERT(whad_board_validate_i2c_result(&resp) == WHAD_SUCCESS);

    /* Boundary: read_data.size = capacity (512) → SUCCESS */
    resp.read_data.size = sizeof(resp.read_data.bytes);
    resp.bytes_read = resp.read_data.size;
    TEST_ASSERT(whad_board_validate_i2c_result(&resp) == WHAD_SUCCESS);

    /* read_data.size over capacity → ERROR */
    resp.read_data.size = (pb_size_t)(sizeof(resp.read_data.bytes) + 1);
    TEST_ASSERT(whad_board_validate_i2c_result(&resp) == WHAD_ERROR);

    /* bytes_read > read_data.size → ERROR */
    resp.read_data.size = 4;
    resp.bytes_read = 8;
    TEST_ASSERT(whad_board_validate_i2c_result(&resp) == WHAD_ERROR);

    /* Bad result → ERROR */
    resp.bytes_read = 0;
    resp.result = (board_BoardResultCode)(_board_BoardResultCode_MAX + 1);
    TEST_ASSERT(whad_board_validate_i2c_result(&resp) == WHAD_ERROR);

    /* Valid lease → SUCCESS */
    resp.result = board_BoardResultCode_SUCCESS;
    resp.has_lease = true;
    resp.lease.resource = board_ResourceKind_RESOURCE_I2C_BUS;
    TEST_ASSERT(whad_board_validate_i2c_result(&resp) == WHAD_SUCCESS);

    /* Bad lease → ERROR */
    resp.lease.resource = (board_ResourceKind)(_board_ResourceKind_MAX + 1);
    TEST_ASSERT(whad_board_validate_i2c_result(&resp) == WHAD_ERROR);

    /* NULL */
    TEST_ASSERT(whad_board_validate_i2c_result(NULL) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_board_validate_gpio_configured — result + lease              */
/* ------------------------------------------------------------------ */

static bool test_validate_gpio_configured(void)
{
    board_GpioConfigureResponse resp;
    memset(&resp, 0, sizeof(resp));

    /* Valid */
    resp.result = board_BoardResultCode_SUCCESS;
    resp.pin = 4;
    TEST_ASSERT(whad_board_validate_gpio_configured(&resp) == WHAD_SUCCESS);

    /* Bad result → ERROR */
    resp.result = (board_BoardResultCode)(_board_BoardResultCode_MAX + 1);
    TEST_ASSERT(whad_board_validate_gpio_configured(&resp) == WHAD_ERROR);

    /* Valid lease → SUCCESS */
    resp.result = board_BoardResultCode_SUCCESS;
    resp.has_lease = true;
    resp.lease.resource = board_ResourceKind_RESOURCE_GPIO_PIN;
    resp.lease.instance = 4;
    TEST_ASSERT(whad_board_validate_gpio_configured(&resp) == WHAD_SUCCESS);

    /* Bad lease → ERROR */
    resp.lease.resource = (board_ResourceKind)(_board_ResourceKind_MAX + 1);
    TEST_ASSERT(whad_board_validate_gpio_configured(&resp) == WHAD_ERROR);

    /* NULL */
    TEST_ASSERT(whad_board_validate_gpio_configured(NULL) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_board_validate_gpio_value — result + lease                   */
/* ------------------------------------------------------------------ */

static bool test_validate_gpio_value(void)
{
    board_GpioReadResponse resp;
    memset(&resp, 0, sizeof(resp));

    /* Valid */
    resp.result = board_BoardResultCode_SUCCESS;
    resp.pin = 4;
    resp.value = true;
    TEST_ASSERT(whad_board_validate_gpio_value(&resp) == WHAD_SUCCESS);

    /* Bad result → ERROR */
    resp.result = (board_BoardResultCode)(_board_BoardResultCode_MAX + 1);
    TEST_ASSERT(whad_board_validate_gpio_value(&resp) == WHAD_ERROR);

    /* Valid lease → SUCCESS */
    resp.result = board_BoardResultCode_SUCCESS;
    resp.has_lease = true;
    resp.lease.resource = board_ResourceKind_RESOURCE_GPIO_PIN;
    resp.lease.instance = 4;
    TEST_ASSERT(whad_board_validate_gpio_value(&resp) == WHAD_SUCCESS);

    /* Bad lease → ERROR */
    resp.lease.resource = (board_ResourceKind)(_board_ResourceKind_MAX + 1);
    TEST_ASSERT(whad_board_validate_gpio_value(&resp) == WHAD_ERROR);

    /* NULL */
    TEST_ASSERT(whad_board_validate_gpio_value(NULL) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_board_validate_adc_value — result enum                       */
/* ------------------------------------------------------------------ */

static bool test_validate_adc_value(void)
{
    board_AdcReadResponse resp;
    memset(&resp, 0, sizeof(resp));

    /* Valid */
    resp.result = board_BoardResultCode_SUCCESS;
    resp.channel = 2;
    resp.millivolts = 1500;
    resp.raw = 512;
    TEST_ASSERT(whad_board_validate_adc_value(&resp) == WHAD_SUCCESS);

    /* Boundary result MIN/MAX */
    resp.result = (board_BoardResultCode)_board_BoardResultCode_MIN;
    TEST_ASSERT(whad_board_validate_adc_value(&resp) == WHAD_SUCCESS);
    resp.result = (board_BoardResultCode)_board_BoardResultCode_MAX;
    TEST_ASSERT(whad_board_validate_adc_value(&resp) == WHAD_SUCCESS);

    /* Out of range → ERROR */
    resp.result = (board_BoardResultCode)(_board_BoardResultCode_MAX + 1);
    TEST_ASSERT(whad_board_validate_adc_value(&resp) == WHAD_ERROR);

    /* NULL */
    TEST_ASSERT(whad_board_validate_adc_value(NULL) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_board_validate_spi_result — result + size + bytes_read + lse */
/* ------------------------------------------------------------------ */

static bool test_validate_spi_result(void)
{
    board_SpiTransferResponse resp;
    memset(&resp, 0, sizeof(resp));

    /* Valid */
    resp.result = board_BoardResultCode_SUCCESS;
    resp.rx_data.size = 4;
    resp.bytes_read = 4;
    TEST_ASSERT(whad_board_validate_spi_result(&resp) == WHAD_SUCCESS);

    /* Boundary: rx_data.size = capacity (512) → SUCCESS */
    resp.rx_data.size = sizeof(resp.rx_data.bytes);
    resp.bytes_read = resp.rx_data.size;
    TEST_ASSERT(whad_board_validate_spi_result(&resp) == WHAD_SUCCESS);

    /* rx_data.size over capacity → ERROR */
    resp.rx_data.size = (pb_size_t)(sizeof(resp.rx_data.bytes) + 1);
    TEST_ASSERT(whad_board_validate_spi_result(&resp) == WHAD_ERROR);

    /* bytes_read > rx_data.size → ERROR */
    resp.rx_data.size = 4;
    resp.bytes_read = 8;
    TEST_ASSERT(whad_board_validate_spi_result(&resp) == WHAD_ERROR);

    /* Bad result → ERROR */
    resp.bytes_read = 0;
    resp.result = (board_BoardResultCode)(_board_BoardResultCode_MAX + 1);
    TEST_ASSERT(whad_board_validate_spi_result(&resp) == WHAD_ERROR);

    /* Valid lease → SUCCESS */
    resp.result = board_BoardResultCode_SUCCESS;
    resp.has_lease = true;
    resp.lease.resource = board_ResourceKind_RESOURCE_SPI_BUS;
    TEST_ASSERT(whad_board_validate_spi_result(&resp) == WHAD_SUCCESS);

    /* Bad lease → ERROR */
    resp.lease.resource = (board_ResourceKind)(_board_ResourceKind_MAX + 1);
    TEST_ASSERT(whad_board_validate_spi_result(&resp) == WHAD_ERROR);

    /* NULL */
    TEST_ASSERT(whad_board_validate_spi_result(NULL) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_board_validate_runtime_config — 2 runtime enums              */
/* ------------------------------------------------------------------ */

static bool test_validate_runtime_config(void)
{
    board_RuntimeConfigResponse resp;
    memset(&resp, 0, sizeof(resp));

    /* Valid: both runtimes known */
    resp.active_runtime = board_RuntimeMode_RUNTIME_RAW_WHAD;
    resp.persisted_runtime = board_RuntimeMode_RUNTIME_BLE_HID;
    resp.persistence_available = false;
    resp.bond_count = 0;
    TEST_ASSERT(whad_board_validate_runtime_config(&resp) == WHAD_SUCCESS);

    /* Boundary MIN */
    resp.active_runtime = (board_RuntimeMode)_board_RuntimeMode_MIN;
    resp.persisted_runtime = (board_RuntimeMode)_board_RuntimeMode_MIN;
    TEST_ASSERT(whad_board_validate_runtime_config(&resp) == WHAD_SUCCESS);

    /* Bad active_runtime → ERROR */
    resp.active_runtime = (board_RuntimeMode)(_board_RuntimeMode_MAX + 1);
    resp.persisted_runtime = board_RuntimeMode_RUNTIME_RAW_WHAD;
    TEST_ASSERT(whad_board_validate_runtime_config(&resp) == WHAD_ERROR);

    /* Bad persisted_runtime → ERROR */
    resp.active_runtime = board_RuntimeMode_RUNTIME_RAW_WHAD;
    resp.persisted_runtime = (board_RuntimeMode)(_board_RuntimeMode_MAX + 1);
    TEST_ASSERT(whad_board_validate_runtime_config(&resp) == WHAD_ERROR);

    /* NULL */
    TEST_ASSERT(whad_board_validate_runtime_config(NULL) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_board_validate_remote_profile — nested profile               */
/* ------------------------------------------------------------------ */

static bool test_validate_remote_profile(void)
{
    board_RemoteProfileResponse resp;
    memset(&resp, 0, sizeof(resp));

    /* No profile → SUCCESS */
    resp.has_profile = false;
    TEST_ASSERT(whad_board_validate_remote_profile(&resp) == WHAD_SUCCESS);

    /* Valid profile → SUCCESS */
    resp.has_profile = true;
    resp.profile.profile_id = 1;
    strncpy(resp.profile.name, "Android TV", sizeof(resp.profile.name) - 1);
    resp.profile.mappings_count = 0;
    TEST_ASSERT(whad_board_validate_remote_profile(&resp) == WHAD_SUCCESS);

    /* name without NUL → ERROR */
    memset(resp.profile.name, 'X', sizeof(resp.profile.name));
    TEST_ASSERT(whad_board_validate_remote_profile(&resp) == WHAD_ERROR);
    resp.profile.name[0] = '\0';

    /* mappings_count over cap (17 > 16) → ERROR */
    resp.profile.mappings_count = 17;
    TEST_ASSERT(whad_board_validate_remote_profile(&resp) == WHAD_ERROR);

    /* mappings_count at cap (16) with valid mappings → SUCCESS */
    resp.profile.mappings_count = 16;
    {
        size_t i;
        for (i = 0; i < 16; i++)
        {
            resp.profile.mappings[i].source = board_InputSource_INPUT_SOURCE_BUTTON_A;
            resp.profile.mappings[i].action = board_InputAction_INPUT_ACTION_PRESS;
        }
    }
    TEST_ASSERT(whad_board_validate_remote_profile(&resp) == WHAD_SUCCESS);

    /* Bad mapping source → ERROR */
    resp.profile.mappings[0].source = (board_InputSource)(_board_InputSource_MAX + 1);
    TEST_ASSERT(whad_board_validate_remote_profile(&resp) == WHAD_ERROR);

    /* NULL */
    TEST_ASSERT(whad_board_validate_remote_profile(NULL) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_board_validate_input_event — source + action enums           */
/* ------------------------------------------------------------------ */

static bool test_validate_input_event(void)
{
    board_InputEvent event;
    memset(&event, 0, sizeof(event));

    /* Valid */
    event.source = board_InputSource_INPUT_SOURCE_BUTTON_A;
    event.action = board_InputAction_INPUT_ACTION_PRESS;
    event.value = 1;
    TEST_ASSERT(whad_board_validate_input_event(&event) == WHAD_SUCCESS);

    /* Boundary source MIN/MAX */
    event.source = (board_InputSource)_board_InputSource_MIN;
    event.action = board_InputAction_INPUT_ACTION_PRESS;
    TEST_ASSERT(whad_board_validate_input_event(&event) == WHAD_SUCCESS);
    event.source = (board_InputSource)_board_InputSource_MAX;
    TEST_ASSERT(whad_board_validate_input_event(&event) == WHAD_SUCCESS);

    /* Bad source → ERROR */
    event.source = (board_InputSource)(_board_InputSource_MAX + 1);
    TEST_ASSERT(whad_board_validate_input_event(&event) == WHAD_ERROR);

    /* Bad action → ERROR */
    event.source = board_InputSource_INPUT_SOURCE_BUTTON_A;
    event.action = (board_InputAction)(_board_InputAction_MAX + 1);
    TEST_ASSERT(whad_board_validate_input_event(&event) == WHAD_ERROR);

    /* NULL */
    TEST_ASSERT(whad_board_validate_input_event(NULL) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* whad_board_validate_gesture_event — gesture enum                  */
/* ------------------------------------------------------------------ */

static bool test_validate_gesture_event(void)
{
    board_GestureEvent event;
    memset(&event, 0, sizeof(event));

    /* Valid */
    event.gesture = board_Gesture_GESTURE_UP;
    event.sequence = 1;
    TEST_ASSERT(whad_board_validate_gesture_event(&event) == WHAD_SUCCESS);

    /* Boundary MIN/MAX */
    event.gesture = (board_Gesture)_board_Gesture_MIN;
    TEST_ASSERT(whad_board_validate_gesture_event(&event) == WHAD_SUCCESS);
    event.gesture = (board_Gesture)_board_Gesture_MAX;
    TEST_ASSERT(whad_board_validate_gesture_event(&event) == WHAD_SUCCESS);

    /* Out of range → ERROR */
    event.gesture = (board_Gesture)(_board_Gesture_MAX + 1);
    TEST_ASSERT(whad_board_validate_gesture_event(&event) == WHAD_ERROR);

    /* NULL */
    TEST_ASSERT(whad_board_validate_gesture_event(NULL) == WHAD_ERROR);
    return true;
}

/* ------------------------------------------------------------------ */
/* main — chain all tests                                            */
/* ------------------------------------------------------------------ */

int main(void)
{
    TEST_ASSERT(test_valid_span_unbounded());
    TEST_ASSERT(test_valid_span_boundary());
    TEST_ASSERT(test_valid_span_overflow_protection());
    TEST_ASSERT(test_validate_lease_null());
    TEST_ASSERT(test_validate_lease_bad_resource());
    TEST_ASSERT(test_validate_lease_valid());
    TEST_ASSERT(test_validate_command_result_null());
    TEST_ASSERT(test_validate_command_result_bad_enum());
    TEST_ASSERT(test_validate_command_result_no_nul());
    TEST_ASSERT(test_validate_command_result_valid());
    TEST_ASSERT(test_command_result_round_trip());
    TEST_ASSERT(test_validate_set_runtime_config());
    TEST_ASSERT(test_validate_audio_chunk());
    TEST_ASSERT(test_validate_log_chunk());
    TEST_ASSERT(test_validate_spi_transfer());
    TEST_ASSERT(test_validate_i2c_transfer());
    TEST_ASSERT(test_validate_board_info());
    TEST_ASSERT(test_validate_board_status());
    TEST_ASSERT(test_validate_remote_profile_set());
    TEST_ASSERT(test_validate_storage_read_log());
    TEST_ASSERT(test_validate_raw_pcm_diagnostics());
    TEST_ASSERT(test_get_message_type());
    TEST_ASSERT(test_command_from_message_type());
    TEST_ASSERT(test_gpio_configure_round_trip());
    TEST_ASSERT(test_validate_storage_status());
    TEST_ASSERT(test_validate_set_runtime_mode());
    TEST_ASSERT(test_validate_set_output());
    TEST_ASSERT(test_validate_configure_input());
    TEST_ASSERT(test_validate_gpio_configure_bounds());
    TEST_ASSERT(test_validate_gpio_read());
    TEST_ASSERT(test_validate_gpio_write());
    TEST_ASSERT(test_validate_release_pin());
    TEST_ASSERT(test_validate_sensor_descriptor());
    TEST_ASSERT(test_validate_sensor_sample());
    TEST_ASSERT(test_validate_calibration());
    TEST_ASSERT(test_validate_input_state());
    TEST_ASSERT(test_validate_i2c_result());
    TEST_ASSERT(test_validate_gpio_configured());
    TEST_ASSERT(test_validate_gpio_value());
    TEST_ASSERT(test_validate_adc_value());
    TEST_ASSERT(test_validate_spi_result());
    TEST_ASSERT(test_validate_runtime_config());
    TEST_ASSERT(test_validate_remote_profile());
    TEST_ASSERT(test_validate_input_event());
    TEST_ASSERT(test_validate_gesture_event());

    printf("All test_board tests passed!\n");
    return 0;
}
