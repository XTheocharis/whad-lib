#include <string.h>

#include <whad.h>
#include <domains/board.h>

typedef whad_result_t (*whad_board_validator_t)(const void *p_value);

static whad_result_t whad_board_validate_ok(const void *p_value)
{
    if (p_value == NULL)
    {
        return WHAD_ERROR;
    }

    return WHAD_SUCCESS;
}

static bool whad_board_valid_string(const char *p_text, size_t capacity)
{
    return memchr(p_text, '\0', capacity) != NULL;
}

static bool whad_board_valid_enum(int value, int min, int max)
{
    return (value >= min) && (value <= max);
}

static bool whad_board_valid_span(uint32_t offset, uint32_t count, uint32_t total)
{
    uint64_t end = (uint64_t)offset + (uint64_t)count;

    return (total == 0) || (end <= total);
}

static whad_result_t whad_board_validate_lease(const board_LeaseToken *p_lease)
{
    if (p_lease == NULL)
    {
        return WHAD_ERROR;
    }

    if (!whad_board_valid_enum(p_lease->resource, _board_ResourceKind_MIN, _board_ResourceKind_MAX))
    {
        return WHAD_ERROR;
    }

    return WHAD_SUCCESS;
}

static whad_result_t whad_board_validate_command_result(const void *p_value)
{
    const board_CommandResult *p_result = (const board_CommandResult *)p_value;

    if (p_result == NULL)
    {
        return WHAD_ERROR;
    }

    if (!whad_board_valid_enum(p_result->command, _board_BoardCommand_MIN, _board_BoardCommand_MAX))
    {
        return WHAD_ERROR;
    }

    if (!whad_board_valid_enum(p_result->result, _board_BoardResultCode_MIN, _board_BoardResultCode_MAX))
    {
        return WHAD_ERROR;
    }
    if (!whad_board_valid_string(p_result->detail, sizeof(p_result->detail)))
    {
        return WHAD_ERROR;
    }

    return WHAD_SUCCESS;
}

static whad_result_t whad_board_validate_board_info(const void *p_value)
{
    const board_GetBoardInfoResponse *p_response = (const board_GetBoardInfoResponse *)p_value;

    if (p_response == NULL)
    {
        return WHAD_ERROR;
    }
    if (!whad_board_valid_string(p_response->board_name, sizeof(p_response->board_name)))
    {
        return WHAD_ERROR;
    }
    if (!whad_board_valid_string(p_response->hardware_revision, sizeof(p_response->hardware_revision)))
    {
        return WHAD_ERROR;
    }
    if (!whad_board_valid_string(p_response->firmware_version, sizeof(p_response->firmware_version)))
    {
        return WHAD_ERROR;
    }
    if (!whad_board_valid_string(p_response->protocol_variant, sizeof(p_response->protocol_variant)))
    {
        return WHAD_ERROR;
    }
    if (p_response->device_id.size > sizeof(p_response->device_id.bytes))
    {
        return WHAD_ERROR;
    }
    if (!whad_board_valid_enum(p_response->active_runtime, _board_RuntimeMode_MIN, _board_RuntimeMode_MAX))
    {
        return WHAD_ERROR;
    }

    return WHAD_SUCCESS;
}

static whad_result_t whad_board_validate_sensor_descriptor_value(const board_SensorDescriptor *p_descriptor)
{
    size_t i;

    if (p_descriptor == NULL)
    {
        return WHAD_ERROR;
    }
    if (!whad_board_valid_string(p_descriptor->name, sizeof(p_descriptor->name)))
    {
        return WHAD_ERROR;
    }
    if (!whad_board_valid_string(p_descriptor->unit, sizeof(p_descriptor->unit)))
    {
        return WHAD_ERROR;
    }
    if (p_descriptor->component_names_count > 10)
    {
        return WHAD_ERROR;
    }
    for (i=0; i<p_descriptor->component_names_count; i++)
    {
        if (!whad_board_valid_string(p_descriptor->component_names[i], sizeof(p_descriptor->component_names[i])))
        {
            return WHAD_ERROR;
        }
    }
    if (p_descriptor->supported_rates_millihz_count > 8)
    {
        return WHAD_ERROR;
    }

    return WHAD_SUCCESS;
}

static whad_result_t whad_board_validate_sensor_descriptor(const void *p_value)
{
    const board_ListSensorsResponse *p_response = (const board_ListSensorsResponse *)p_value;

    if (p_response == NULL)
    {
        return WHAD_ERROR;
    }
    if (p_response->has_descriptor)
    {
        return whad_board_validate_sensor_descriptor_value(&p_response->descriptor);
    }

    return WHAD_SUCCESS;
}

static whad_result_t whad_board_validate_sensor_sample(const void *p_value)
{
    const board_SensorSample *p_sample = (const board_SensorSample *)p_value;

    if (p_sample == NULL)
    {
        return WHAD_ERROR;
    }
    if (p_sample->values_count > 10)
    {
        return WHAD_ERROR;
    }
    if ((p_sample->status & ~((uint32_t)board_SensorStatusFlag_SENSOR_STATUS_SENSOR_FAULT | (uint32_t)board_SensorStatusFlag_SENSOR_STATUS_MAGNETIC_DISTURBANCE | (uint32_t)board_SensorStatusFlag_SENSOR_STATUS_OVERRUN | (uint32_t)board_SensorStatusFlag_SENSOR_STATUS_SATURATED | (uint32_t)board_SensorStatusFlag_SENSOR_STATUS_STALE | (uint32_t)board_SensorStatusFlag_SENSOR_STATUS_CALIBRATING | (uint32_t)board_SensorStatusFlag_SENSOR_STATUS_CALIBRATED)) != 0)
    {
        return WHAD_ERROR;
    }

    return WHAD_SUCCESS;
}

static whad_result_t whad_board_validate_calibration(const void *p_value)
{
    const board_CalibrationResponse *p_response = (const board_CalibrationResponse *)p_value;

    if (p_response == NULL)
    {
        return WHAD_ERROR;
    }
    if (p_response->calibration_data.size > sizeof(p_response->calibration_data.bytes))
    {
        return WHAD_ERROR;
    }

    return WHAD_SUCCESS;
}

static whad_result_t whad_board_validate_set_output(const void *p_value)
{
    const board_SetOutputRequest *p_request = (const board_SetOutputRequest *)p_value;

    if (p_request == NULL)
    {
        return WHAD_ERROR;
    }
    if (!whad_board_valid_enum(p_request->target, _board_OutputTarget_MIN, _board_OutputTarget_MAX))
    {
        return WHAD_ERROR;
    }
    if (p_request->has_lease)
    {
        return whad_board_validate_lease(&p_request->lease);
    }

    return WHAD_SUCCESS;
}

static whad_result_t whad_board_validate_input_state(const void *p_value)
{
    const board_InputStateResponse *p_response = (const board_InputStateResponse *)p_value;

    if (p_response == NULL)
    {
        return WHAD_ERROR;
    }
    if (!whad_board_valid_enum(p_response->gesture, _board_Gesture_MIN, _board_Gesture_MAX))
    {
        return WHAD_ERROR;
    }
    if (!whad_board_valid_enum(p_response->mode, _board_InputMode_MIN, _board_InputMode_MAX))
    {
        return WHAD_ERROR;
    }

    return WHAD_SUCCESS;
}

static whad_result_t whad_board_validate_configure_input(const void *p_value)
{
    const board_ConfigureInputRequest *p_request = (const board_ConfigureInputRequest *)p_value;

    if (p_request == NULL)
    {
        return WHAD_ERROR;
    }
    if (!whad_board_valid_enum(p_request->mode, _board_InputMode_MIN, _board_InputMode_MAX))
    {
        return WHAD_ERROR;
    }

    return WHAD_SUCCESS;
}

static whad_result_t whad_board_validate_i2c_transfer(const void *p_value)
{
    const board_I2cTransferRequest *p_request = (const board_I2cTransferRequest *)p_value;

    if (p_request == NULL)
    {
        return WHAD_ERROR;
    }
    if (p_request->write_data.size > sizeof(p_request->write_data.bytes))
    {
        return WHAD_ERROR;
    }
    if (p_request->read_length > sizeof(((board_I2cTransferResponse *)0)->read_data.bytes))
    {
        return WHAD_ERROR;
    }
    if (p_request->has_lease)
    {
        return whad_board_validate_lease(&p_request->lease);
    }

    return WHAD_SUCCESS;
}

static whad_result_t whad_board_validate_i2c_result(const void *p_value)
{
    const board_I2cTransferResponse *p_response = (const board_I2cTransferResponse *)p_value;

    if (p_response == NULL)
    {
        return WHAD_ERROR;
    }
    if (!whad_board_valid_enum(p_response->result, _board_BoardResultCode_MIN, _board_BoardResultCode_MAX))
    {
        return WHAD_ERROR;
    }
    if (p_response->read_data.size > sizeof(p_response->read_data.bytes))
    {
        return WHAD_ERROR;
    }
    if (p_response->bytes_read > p_response->read_data.size)
    {
        return WHAD_ERROR;
    }
    if (p_response->has_lease)
    {
        return whad_board_validate_lease(&p_response->lease);
    }

    return WHAD_SUCCESS;
}

static whad_result_t whad_board_validate_gpio_configure(const void *p_value)
{
    const board_GpioConfigureRequest *p_request = (const board_GpioConfigureRequest *)p_value;

    if (p_request == NULL)
    {
        return WHAD_ERROR;
    }
    if (!whad_board_valid_enum(p_request->direction, _board_GpioDirection_MIN, _board_GpioDirection_MAX))
    {
        return WHAD_ERROR;
    }
    if (!whad_board_valid_enum(p_request->pull, _board_GpioPull_MIN, _board_GpioPull_MAX))
    {
        return WHAD_ERROR;
    }
    if (p_request->has_lease)
    {
        return whad_board_validate_lease(&p_request->lease);
    }

    return WHAD_SUCCESS;
}

static whad_result_t whad_board_validate_gpio_read(const void *p_value)
{
    const board_GpioReadRequest *p_request = (const board_GpioReadRequest *)p_value;

    if (p_request == NULL)
    {
        return WHAD_ERROR;
    }
    if (p_request->has_lease)
    {
        return whad_board_validate_lease(&p_request->lease);
    }

    return WHAD_SUCCESS;
}

static whad_result_t whad_board_validate_gpio_write(const void *p_value)
{
    const board_GpioWriteRequest *p_request = (const board_GpioWriteRequest *)p_value;

    if (p_request == NULL)
    {
        return WHAD_ERROR;
    }
    if (p_request->has_lease)
    {
        return whad_board_validate_lease(&p_request->lease);
    }

    return WHAD_SUCCESS;
}

static whad_result_t whad_board_validate_gpio_configured(const void *p_value)
{
    const board_GpioConfigureResponse *p_response = (const board_GpioConfigureResponse *)p_value;

    if (p_response == NULL)
    {
        return WHAD_ERROR;
    }
    if (!whad_board_valid_enum(p_response->result, _board_BoardResultCode_MIN, _board_BoardResultCode_MAX))
    {
        return WHAD_ERROR;
    }
    if (p_response->has_lease)
    {
        return whad_board_validate_lease(&p_response->lease);
    }

    return WHAD_SUCCESS;
}

static whad_result_t whad_board_validate_gpio_value(const void *p_value)
{
    const board_GpioReadResponse *p_response = (const board_GpioReadResponse *)p_value;

    if (p_response == NULL)
    {
        return WHAD_ERROR;
    }
    if (!whad_board_valid_enum(p_response->result, _board_BoardResultCode_MIN, _board_BoardResultCode_MAX))
    {
        return WHAD_ERROR;
    }
    if (p_response->has_lease)
    {
        return whad_board_validate_lease(&p_response->lease);
    }

    return WHAD_SUCCESS;
}

static whad_result_t whad_board_validate_adc_value(const void *p_value)
{
    const board_AdcReadResponse *p_response = (const board_AdcReadResponse *)p_value;

    if (p_response == NULL)
    {
        return WHAD_ERROR;
    }
    if (!whad_board_valid_enum(p_response->result, _board_BoardResultCode_MIN, _board_BoardResultCode_MAX))
    {
        return WHAD_ERROR;
    }

    return WHAD_SUCCESS;
}

static whad_result_t whad_board_validate_spi_transfer(const void *p_value)
{
    const board_SpiTransferRequest *p_request = (const board_SpiTransferRequest *)p_value;

    if (p_request == NULL)
    {
        return WHAD_ERROR;
    }
    if (!whad_board_valid_enum(p_request->mode, _board_SpiMode_MIN, _board_SpiMode_MAX))
    {
        return WHAD_ERROR;
    }
    if (p_request->tx_data.size > sizeof(p_request->tx_data.bytes))
    {
        return WHAD_ERROR;
    }
    if (p_request->read_length > sizeof(((board_SpiTransferResponse *)0)->rx_data.bytes))
    {
        return WHAD_ERROR;
    }
    if (p_request->has_lease)
    {
        return whad_board_validate_lease(&p_request->lease);
    }

    return WHAD_SUCCESS;
}

static whad_result_t whad_board_validate_spi_result(const void *p_value)
{
    const board_SpiTransferResponse *p_response = (const board_SpiTransferResponse *)p_value;

    if (p_response == NULL)
    {
        return WHAD_ERROR;
    }
    if (!whad_board_valid_enum(p_response->result, _board_BoardResultCode_MIN, _board_BoardResultCode_MAX))
    {
        return WHAD_ERROR;
    }
    if (p_response->rx_data.size > sizeof(p_response->rx_data.bytes))
    {
        return WHAD_ERROR;
    }
    if (p_response->bytes_read > p_response->rx_data.size)
    {
        return WHAD_ERROR;
    }
    if (p_response->has_lease)
    {
        return whad_board_validate_lease(&p_response->lease);
    }

    return WHAD_SUCCESS;
}

static whad_result_t whad_board_validate_storage_status(const void *p_value)
{
    const board_StorageInfoResponse *p_response = (const board_StorageInfoResponse *)p_value;

    if (p_response == NULL)
    {
        return WHAD_ERROR;
    }
    if (!whad_board_valid_enum(p_response->state, _board_StorageState_MIN, _board_StorageState_MAX))
    {
        return WHAD_ERROR;
    }

    return WHAD_SUCCESS;
}

static whad_result_t whad_board_validate_storage_read_log(const void *p_value)
{
    const board_StorageReadLogRequest *p_request = (const board_StorageReadLogRequest *)p_value;

    if (p_request == NULL)
    {
        return WHAD_ERROR;
    }
    if (p_request->max_bytes > sizeof(((board_LogChunk *)0)->data.bytes))
    {
        return WHAD_ERROR;
    }

    return WHAD_SUCCESS;
}

static whad_result_t whad_board_validate_runtime_config(const void *p_value)
{
    const board_RuntimeConfigResponse *p_response = (const board_RuntimeConfigResponse *)p_value;

    if (p_response == NULL)
    {
        return WHAD_ERROR;
    }
    if (!whad_board_valid_enum(p_response->active_runtime, _board_RuntimeMode_MIN, _board_RuntimeMode_MAX))
    {
        return WHAD_ERROR;
    }
    if (!whad_board_valid_enum(p_response->persisted_runtime, _board_RuntimeMode_MIN, _board_RuntimeMode_MAX))
    {
        return WHAD_ERROR;
    }

    return WHAD_SUCCESS;
}

static whad_result_t whad_board_validate_runtime_update(const board_RuntimeConfigUpdate *p_update)
{
    if (p_update == NULL)
    {
        return WHAD_ERROR;
    }
    if (p_update->has_persisted_runtime && !whad_board_valid_enum(p_update->persisted_runtime, _board_RuntimeMode_MIN, _board_RuntimeMode_MAX))
    {
        return WHAD_ERROR;
    }

    return WHAD_SUCCESS;
}

static whad_result_t whad_board_validate_set_runtime_config(const void *p_value)
{
    const board_SetRuntimeConfigRequest *p_request = (const board_SetRuntimeConfigRequest *)p_value;

    if (p_request == NULL)
    {
        return WHAD_ERROR;
    }

    switch (p_request->which_operation)
    {
        case board_SetRuntimeConfigRequest_update_tag:
            return whad_board_validate_runtime_update(&p_request->operation.update);

        case board_SetRuntimeConfigRequest_open_pairing_window_tag:
        case board_SetRuntimeConfigRequest_clear_bonds_tag:
            return WHAD_SUCCESS;

        default:
            return WHAD_ERROR;
    }
}

static whad_result_t whad_board_validate_set_runtime_mode(const void *p_value)
{
    const board_SetRuntimeModeRequest *p_request = (const board_SetRuntimeModeRequest *)p_value;

    if (p_request == NULL)
    {
        return WHAD_ERROR;
    }
    if (!whad_board_valid_enum(p_request->runtime, _board_RuntimeMode_MIN, _board_RuntimeMode_MAX))
    {
        return WHAD_ERROR;
    }

    return WHAD_SUCCESS;
}

static whad_result_t whad_board_validate_profile_mapping(const board_ProfileMapping *p_mapping)
{
    if (p_mapping == NULL)
    {
        return WHAD_ERROR;
    }
    if (!whad_board_valid_enum(p_mapping->source, _board_InputSource_MIN, _board_InputSource_MAX))
    {
        return WHAD_ERROR;
    }
    if (!whad_board_valid_enum(p_mapping->action, _board_InputAction_MIN, _board_InputAction_MAX))
    {
        return WHAD_ERROR;
    }

    return WHAD_SUCCESS;
}

static whad_result_t whad_board_validate_profile(const board_RemoteProfile *p_profile)
{
    size_t i;

    if (p_profile == NULL)
    {
        return WHAD_ERROR;
    }
    if (!whad_board_valid_string(p_profile->name, sizeof(p_profile->name)))
    {
        return WHAD_ERROR;
    }
    if (p_profile->mappings_count > 16)
    {
        return WHAD_ERROR;
    }
    for (i=0; i<p_profile->mappings_count; i++)
    {
        if (whad_board_validate_profile_mapping(&p_profile->mappings[i]) != WHAD_SUCCESS)
        {
            return WHAD_ERROR;
        }
    }

    return WHAD_SUCCESS;
}

static whad_result_t whad_board_validate_remote_profile(const void *p_value)
{
    const board_RemoteProfileResponse *p_response = (const board_RemoteProfileResponse *)p_value;

    if (p_response == NULL)
    {
        return WHAD_ERROR;
    }
    if (p_response->has_profile)
    {
        return whad_board_validate_profile(&p_response->profile);
    }

    return WHAD_SUCCESS;
}

static whad_result_t whad_board_validate_remote_profile_set(const void *p_value)
{
    const board_RemoteProfileSetRequest *p_request = (const board_RemoteProfileSetRequest *)p_value;

    if (p_request == NULL)
    {
        return WHAD_ERROR;
    }
    if (p_request->has_profile)
    {
        return whad_board_validate_profile(&p_request->profile);
    }

    return WHAD_SUCCESS;
}

static whad_result_t whad_board_validate_release_pin(const void *p_value)
{
    const board_ReleasePinRequest *p_request = (const board_ReleasePinRequest *)p_value;

    if (p_request == NULL)
    {
        return WHAD_ERROR;
    }
    if (p_request->has_lease)
    {
        return whad_board_validate_lease(&p_request->lease);
    }
    if (!whad_board_valid_enum(p_request->resource, _board_ResourceKind_MIN, _board_ResourceKind_MAX))
    {
        return WHAD_ERROR;
    }

    return WHAD_SUCCESS;
}

static whad_result_t whad_board_validate_audio_chunk(const void *p_value)
{
    const board_AudioChunk *p_chunk = (const board_AudioChunk *)p_value;

    if (p_chunk == NULL)
    {
        return WHAD_ERROR;
    }
    if (!whad_board_valid_enum(p_chunk->result, _board_BoardResultCode_MIN, _board_BoardResultCode_MAX))
    {
        return WHAD_ERROR;
    }
    if (p_chunk->pcm.size > sizeof(p_chunk->pcm.bytes))
    {
        return WHAD_ERROR;
    }
    if (p_chunk->count > p_chunk->pcm.size)
    {
        return WHAD_ERROR;
    }
    if (!whad_board_valid_span(p_chunk->offset, p_chunk->count, p_chunk->total))
    {
        return WHAD_ERROR;
    }

    return WHAD_SUCCESS;
}

static whad_result_t whad_board_validate_input_event(const void *p_value)
{
    const board_InputEvent *p_event = (const board_InputEvent *)p_value;

    if (p_event == NULL)
    {
        return WHAD_ERROR;
    }
    if (!whad_board_valid_enum(p_event->source, _board_InputSource_MIN, _board_InputSource_MAX))
    {
        return WHAD_ERROR;
    }
    if (!whad_board_valid_enum(p_event->action, _board_InputAction_MIN, _board_InputAction_MAX))
    {
        return WHAD_ERROR;
    }

    return WHAD_SUCCESS;
}

static whad_result_t whad_board_validate_gesture_event(const void *p_value)
{
    const board_GestureEvent *p_event = (const board_GestureEvent *)p_value;

    if (p_event == NULL)
    {
        return WHAD_ERROR;
    }
    if (!whad_board_valid_enum(p_event->gesture, _board_Gesture_MIN, _board_Gesture_MAX))
    {
        return WHAD_ERROR;
    }

    return WHAD_SUCCESS;
}

static whad_result_t whad_board_validate_log_chunk(const void *p_value)
{
    const board_LogChunk *p_chunk = (const board_LogChunk *)p_value;

    if (p_chunk == NULL)
    {
        return WHAD_ERROR;
    }
    if (!whad_board_valid_enum(p_chunk->result, _board_BoardResultCode_MIN, _board_BoardResultCode_MAX))
    {
        return WHAD_ERROR;
    }
    if (p_chunk->data.size > sizeof(p_chunk->data.bytes))
    {
        return WHAD_ERROR;
    }
    if (p_chunk->count > p_chunk->data.size)
    {
        return WHAD_ERROR;
    }
    if (!whad_board_valid_span(p_chunk->offset, p_chunk->count, p_chunk->total))
    {
        return WHAD_ERROR;
    }

    return WHAD_SUCCESS;
}

static whad_result_t whad_board_validate_board_status(const void *p_value)
{
    const board_BoardStatus *p_status = (const board_BoardStatus *)p_value;

    if (p_status == NULL)
    {
        return WHAD_ERROR;
    }
    if (!whad_board_valid_enum(p_status->code, _board_BoardStatusCode_MIN, _board_BoardStatusCode_MAX))
    {
        return WHAD_ERROR;
    }
    if (!whad_board_valid_enum(p_status->result, _board_BoardResultCode_MIN, _board_BoardResultCode_MAX))
    {
        return WHAD_ERROR;
    }
    if (!whad_board_valid_enum(p_status->resource, _board_ResourceKind_MIN, _board_ResourceKind_MAX))
    {
        return WHAD_ERROR;
    }
    if (p_status->progress_per_mille > 1000)
    {
        return WHAD_ERROR;
    }
    if (!whad_board_valid_string(p_status->detail, sizeof(p_status->detail)))
    {
        return WHAD_ERROR;
    }

    return WHAD_SUCCESS;
}

static whad_result_t whad_board_validate_raw_pcm_diagnostics(const void *p_value)
{
    const board_RawPcmDiagnosticsRequest *p_request = (const board_RawPcmDiagnosticsRequest *)p_value;

    if (p_request == NULL)
    {
        return WHAD_ERROR;
    }
    if (p_request->chunk_size > sizeof(((board_AudioChunk *)0)->pcm.bytes))
    {
        return WHAD_ERROR;
    }

    return WHAD_SUCCESS;
}

static whad_result_t whad_board_pack(
    Message *p_message,
    uint32_t request_id,
    pb_size_t tag,
    void *p_field,
    size_t field_size,
    const void *p_value,
    whad_board_validator_t validator)
{
    if ((p_message == NULL) || (p_field == NULL) || (p_value == NULL) || (validator == NULL))
    {
        return WHAD_ERROR;
    }
    if (validator(p_value) != WHAD_SUCCESS)
    {
        return WHAD_ERROR;
    }

    p_message->which_msg = 0;
    memset(&p_message->msg.board, 0, sizeof(board_Message));
    p_message->which_msg = Message_board_tag;
    p_message->msg.board.request_id = request_id;
    p_message->msg.board.which_msg = tag;
    memcpy(p_field, p_value, field_size);

    return WHAD_SUCCESS;
}

static whad_result_t whad_board_unpack(
    Message *p_message,
    pb_size_t tag,
    const void *p_field,
    size_t field_size,
    uint32_t *p_request_id,
    void *p_value,
    whad_board_validator_t validator)
{
    if ((p_message == NULL) || (p_field == NULL) || (p_value == NULL) || (validator == NULL))
    {
        return WHAD_ERROR;
    }
    if (whad_get_message_domain(p_message) != DOMAIN_BOARD)
    {
        return WHAD_ERROR;
    }
    if (p_message->msg.board.which_msg != tag)
    {
        return WHAD_ERROR;
    }
    if (validator(p_field) != WHAD_SUCCESS)
    {
        return WHAD_ERROR;
    }

    if (p_request_id != NULL)
    {
        *p_request_id = p_message->msg.board.request_id;
    }
    memcpy(p_value, p_field, field_size);

    return WHAD_SUCCESS;
}

whad_board_msgtype_t whad_board_get_message_type(Message *p_message)
{
    if ((p_message == NULL) || (whad_get_message_domain(p_message) != DOMAIN_BOARD))
    {
        return WHAD_BOARD_UNKNOWN;
    }

    return (whad_board_msgtype_t)p_message->msg.board.which_msg;
}

whad_result_t whad_board_command_from_message_type(whad_board_msgtype_t msg_type, board_BoardCommand *p_command)
{
    if (p_command == NULL)
    {
        return WHAD_ERROR;
    }

    switch (msg_type)
    {
        case WHAD_BOARD_GET_BOARD_INFO:
            *p_command = board_BoardCommand_GetBoardInfo;
            return WHAD_SUCCESS;
        case WHAD_BOARD_LIST_SENSORS:
            *p_command = board_BoardCommand_ListSensors;
            return WHAD_SUCCESS;
        case WHAD_BOARD_READ_SENSOR:
            *p_command = board_BoardCommand_ReadSensor;
            return WHAD_SUCCESS;
        case WHAD_BOARD_CONFIGURE_STREAM:
            *p_command = board_BoardCommand_ConfigureStream;
            return WHAD_SUCCESS;
        case WHAD_BOARD_STOP_STREAM:
            *p_command = board_BoardCommand_StopStream;
            return WHAD_SUCCESS;
        case WHAD_BOARD_CALIBRATE:
            *p_command = board_BoardCommand_Calibrate;
            return WHAD_SUCCESS;
        case WHAD_BOARD_GET_CALIBRATION:
            *p_command = board_BoardCommand_GetCalibration;
            return WHAD_SUCCESS;
        case WHAD_BOARD_SET_OUTPUT:
            *p_command = board_BoardCommand_SetOutput;
            return WHAD_SUCCESS;
        case WHAD_BOARD_GET_INPUT_STATE:
            *p_command = board_BoardCommand_GetInputState;
            return WHAD_SUCCESS;
        case WHAD_BOARD_CONFIGURE_INPUT:
            *p_command = board_BoardCommand_ConfigureInput;
            return WHAD_SUCCESS;
        case WHAD_BOARD_I2C_TRANSFER:
            *p_command = board_BoardCommand_I2cTransfer;
            return WHAD_SUCCESS;
        case WHAD_BOARD_GPIO_CONFIGURE:
            *p_command = board_BoardCommand_GpioConfigure;
            return WHAD_SUCCESS;
        case WHAD_BOARD_GPIO_READ:
            *p_command = board_BoardCommand_GpioRead;
            return WHAD_SUCCESS;
        case WHAD_BOARD_GPIO_WRITE:
            *p_command = board_BoardCommand_GpioWrite;
            return WHAD_SUCCESS;
        case WHAD_BOARD_ADC_READ:
            *p_command = board_BoardCommand_AdcRead;
            return WHAD_SUCCESS;
        case WHAD_BOARD_SPI_TRANSFER:
            *p_command = board_BoardCommand_SpiTransfer;
            return WHAD_SUCCESS;
        case WHAD_BOARD_STORAGE_INFO:
            *p_command = board_BoardCommand_StorageInfo;
            return WHAD_SUCCESS;
        case WHAD_BOARD_STORAGE_ADOPT:
            *p_command = board_BoardCommand_StorageAdopt;
            return WHAD_SUCCESS;
        case WHAD_BOARD_STORAGE_READ_LOG:
            *p_command = board_BoardCommand_StorageReadLog;
            return WHAD_SUCCESS;
        case WHAD_BOARD_STORAGE_ERASE_LOG:
            *p_command = board_BoardCommand_StorageEraseLog;
            return WHAD_SUCCESS;
        case WHAD_BOARD_GET_RUNTIME_CONFIG:
            *p_command = board_BoardCommand_GetRuntimeConfig;
            return WHAD_SUCCESS;
        case WHAD_BOARD_SET_RUNTIME_CONFIG:
            *p_command = board_BoardCommand_SetRuntimeConfig;
            return WHAD_SUCCESS;
        case WHAD_BOARD_SET_RUNTIME_MODE:
            *p_command = board_BoardCommand_SetRuntimeMode;
            return WHAD_SUCCESS;
        case WHAD_BOARD_REMOTE_PROFILE_GET:
            *p_command = board_BoardCommand_RemoteProfileGet;
            return WHAD_SUCCESS;
        case WHAD_BOARD_REMOTE_PROFILE_SET:
            *p_command = board_BoardCommand_RemoteProfileSet;
            return WHAD_SUCCESS;
        case WHAD_BOARD_AUDIO_CONFIGURE:
            *p_command = board_BoardCommand_AudioConfigure;
            return WHAD_SUCCESS;
        case WHAD_BOARD_RELEASE_PIN:
            *p_command = board_BoardCommand_ReleasePin;
            return WHAD_SUCCESS;
        case WHAD_BOARD_RAW_PCM_DIAGNOSTICS:
            *p_command = board_BoardCommand_RawPcmDiagnostics;
            return WHAD_SUCCESS;
        default:
            return WHAD_ERROR;
    }
}

#define WHAD_BOARD_VALIDATE_get_board_info whad_board_validate_ok
#define WHAD_BOARD_VALIDATE_list_sensors whad_board_validate_ok
#define WHAD_BOARD_VALIDATE_read_sensor whad_board_validate_ok
#define WHAD_BOARD_VALIDATE_configure_stream whad_board_validate_ok
#define WHAD_BOARD_VALIDATE_stop_stream whad_board_validate_ok
#define WHAD_BOARD_VALIDATE_calibrate whad_board_validate_ok
#define WHAD_BOARD_VALIDATE_get_calibration whad_board_validate_ok
#define WHAD_BOARD_VALIDATE_set_output whad_board_validate_set_output
#define WHAD_BOARD_VALIDATE_get_input_state whad_board_validate_ok
#define WHAD_BOARD_VALIDATE_configure_input whad_board_validate_configure_input
#define WHAD_BOARD_VALIDATE_i2c_transfer whad_board_validate_i2c_transfer
#define WHAD_BOARD_VALIDATE_gpio_configure whad_board_validate_gpio_configure
#define WHAD_BOARD_VALIDATE_gpio_read whad_board_validate_gpio_read
#define WHAD_BOARD_VALIDATE_gpio_write whad_board_validate_gpio_write
#define WHAD_BOARD_VALIDATE_adc_read whad_board_validate_ok
#define WHAD_BOARD_VALIDATE_spi_transfer whad_board_validate_spi_transfer
#define WHAD_BOARD_VALIDATE_storage_info whad_board_validate_ok
#define WHAD_BOARD_VALIDATE_storage_adopt whad_board_validate_ok
#define WHAD_BOARD_VALIDATE_storage_read_log whad_board_validate_storage_read_log
#define WHAD_BOARD_VALIDATE_storage_erase_log whad_board_validate_ok
#define WHAD_BOARD_VALIDATE_get_runtime_config whad_board_validate_ok
#define WHAD_BOARD_VALIDATE_set_runtime_config whad_board_validate_set_runtime_config
#define WHAD_BOARD_VALIDATE_set_runtime_mode whad_board_validate_set_runtime_mode
#define WHAD_BOARD_VALIDATE_remote_profile_get whad_board_validate_ok
#define WHAD_BOARD_VALIDATE_remote_profile_set whad_board_validate_remote_profile_set
#define WHAD_BOARD_VALIDATE_audio_configure whad_board_validate_ok
#define WHAD_BOARD_VALIDATE_release_pin whad_board_validate_release_pin
#define WHAD_BOARD_VALIDATE_raw_pcm_diagnostics whad_board_validate_raw_pcm_diagnostics
#define WHAD_BOARD_VALIDATE_command_result whad_board_validate_command_result
#define WHAD_BOARD_VALIDATE_board_info whad_board_validate_board_info
#define WHAD_BOARD_VALIDATE_sensor_descriptor whad_board_validate_sensor_descriptor
#define WHAD_BOARD_VALIDATE_sensor_sample whad_board_validate_sensor_sample
#define WHAD_BOARD_VALIDATE_stream_configured whad_board_validate_ok
#define WHAD_BOARD_VALIDATE_calibration whad_board_validate_calibration
#define WHAD_BOARD_VALIDATE_input_state whad_board_validate_input_state
#define WHAD_BOARD_VALIDATE_i2c_result whad_board_validate_i2c_result
#define WHAD_BOARD_VALIDATE_gpio_configured whad_board_validate_gpio_configured
#define WHAD_BOARD_VALIDATE_gpio_value whad_board_validate_gpio_value
#define WHAD_BOARD_VALIDATE_adc_value whad_board_validate_adc_value
#define WHAD_BOARD_VALIDATE_spi_result whad_board_validate_spi_result
#define WHAD_BOARD_VALIDATE_storage_status whad_board_validate_storage_status
#define WHAD_BOARD_VALIDATE_runtime_config whad_board_validate_runtime_config
#define WHAD_BOARD_VALIDATE_remote_profile whad_board_validate_remote_profile
#define WHAD_BOARD_VALIDATE_audio_configured whad_board_validate_ok
#define WHAD_BOARD_VALIDATE_audio_chunk whad_board_validate_audio_chunk
#define WHAD_BOARD_VALIDATE_input_event whad_board_validate_input_event
#define WHAD_BOARD_VALIDATE_gesture_event whad_board_validate_gesture_event
#define WHAD_BOARD_VALIDATE_log_chunk whad_board_validate_log_chunk
#define WHAD_BOARD_VALIDATE_board_status whad_board_validate_board_status

#define WHAD_BOARD_DEFINE_MESSAGE(name, tag, field, type) \
    whad_result_t whad_board_##name(Message *p_message, uint32_t request_id, const type *p_value) \
    { \
        return whad_board_pack(p_message, request_id, tag, &p_message->msg.board.msg.field, sizeof(type), p_value, WHAD_BOARD_VALIDATE_##name); \
    } \
    whad_result_t whad_board_##name##_parse(Message *p_message, uint32_t *p_request_id, type *p_value) \
    { \
        return whad_board_unpack(p_message, tag, &p_message->msg.board.msg.field, sizeof(type), p_request_id, p_value, WHAD_BOARD_VALIDATE_##name); \
    }

WHAD_BOARD_MESSAGE_LIST(WHAD_BOARD_DEFINE_MESSAGE)

#undef WHAD_BOARD_DEFINE_MESSAGE
