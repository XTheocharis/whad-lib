#ifndef __INC_WHAD_BOARD_H
#define __INC_WHAD_BOARD_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    WHAD_BOARD_UNKNOWN=0,
    WHAD_BOARD_GET_BOARD_INFO=board_Message_get_board_info_tag,
    WHAD_BOARD_LIST_SENSORS=board_Message_list_sensors_tag,
    WHAD_BOARD_READ_SENSOR=board_Message_read_sensor_tag,
    WHAD_BOARD_CONFIGURE_STREAM=board_Message_configure_stream_tag,
    WHAD_BOARD_STOP_STREAM=board_Message_stop_stream_tag,
    WHAD_BOARD_CALIBRATE=board_Message_calibrate_tag,
    WHAD_BOARD_GET_CALIBRATION=board_Message_get_calibration_tag,
    WHAD_BOARD_SET_OUTPUT=board_Message_set_output_tag,
    WHAD_BOARD_GET_INPUT_STATE=board_Message_get_input_state_tag,
    WHAD_BOARD_CONFIGURE_INPUT=board_Message_configure_input_tag,
    WHAD_BOARD_I2C_TRANSFER=board_Message_i2c_transfer_tag,
    WHAD_BOARD_GPIO_CONFIGURE=board_Message_gpio_configure_tag,
    WHAD_BOARD_GPIO_READ=board_Message_gpio_read_tag,
    WHAD_BOARD_GPIO_WRITE=board_Message_gpio_write_tag,
    WHAD_BOARD_ADC_READ=board_Message_adc_read_tag,
    WHAD_BOARD_SPI_TRANSFER=board_Message_spi_transfer_tag,
    WHAD_BOARD_STORAGE_INFO=board_Message_storage_info_tag,
    WHAD_BOARD_STORAGE_ADOPT=board_Message_storage_adopt_tag,
    WHAD_BOARD_STORAGE_READ_LOG=board_Message_storage_read_log_tag,
    WHAD_BOARD_STORAGE_ERASE_LOG=board_Message_storage_erase_log_tag,
    WHAD_BOARD_GET_RUNTIME_CONFIG=board_Message_get_runtime_config_tag,
    WHAD_BOARD_SET_RUNTIME_CONFIG=board_Message_set_runtime_config_tag,
    WHAD_BOARD_SET_RUNTIME_MODE=board_Message_set_runtime_mode_tag,
    WHAD_BOARD_REMOTE_PROFILE_GET=board_Message_remote_profile_get_tag,
    WHAD_BOARD_REMOTE_PROFILE_SET=board_Message_remote_profile_set_tag,
    WHAD_BOARD_AUDIO_CONFIGURE=board_Message_audio_configure_tag,
    WHAD_BOARD_RELEASE_PIN=board_Message_release_pin_tag,
    WHAD_BOARD_RAW_PCM_DIAGNOSTICS=board_Message_raw_pcm_diagnostics_tag,
    WHAD_BOARD_COMMAND_RESULT=board_Message_command_result_tag,
    WHAD_BOARD_INFO=board_Message_board_info_tag,
    WHAD_BOARD_SENSOR_DESCRIPTOR=board_Message_sensor_descriptor_tag,
    WHAD_BOARD_SENSOR_SAMPLE=board_Message_sensor_sample_tag,
    WHAD_BOARD_STREAM_CONFIGURED=board_Message_stream_configured_tag,
    WHAD_BOARD_CALIBRATION=board_Message_calibration_tag,
    WHAD_BOARD_INPUT_STATE=board_Message_input_state_tag,
    WHAD_BOARD_I2C_RESULT=board_Message_i2c_result_tag,
    WHAD_BOARD_GPIO_CONFIGURED=board_Message_gpio_configured_tag,
    WHAD_BOARD_GPIO_VALUE=board_Message_gpio_value_tag,
    WHAD_BOARD_ADC_VALUE=board_Message_adc_value_tag,
    WHAD_BOARD_SPI_RESULT=board_Message_spi_result_tag,
    WHAD_BOARD_STORAGE_STATUS=board_Message_storage_status_tag,
    WHAD_BOARD_RUNTIME_CONFIG=board_Message_runtime_config_tag,
    WHAD_BOARD_REMOTE_PROFILE=board_Message_remote_profile_tag,
    WHAD_BOARD_AUDIO_CONFIGURED=board_Message_audio_configured_tag,
    WHAD_BOARD_AUDIO_CHUNK=board_Message_audio_chunk_tag,
    WHAD_BOARD_INPUT_EVENT=board_Message_input_event_tag,
    WHAD_BOARD_GESTURE_EVENT=board_Message_gesture_event_tag,
    WHAD_BOARD_LOG_CHUNK=board_Message_log_chunk_tag,
    WHAD_BOARD_STATUS=board_Message_board_status_tag
} whad_board_msgtype_t;

typedef enum {
    WHAD_BOARD_CMD_GET_BOARD_INFO=board_BoardCommand_GetBoardInfo,
    WHAD_BOARD_CMD_LIST_SENSORS=board_BoardCommand_ListSensors,
    WHAD_BOARD_CMD_READ_SENSOR=board_BoardCommand_ReadSensor,
    WHAD_BOARD_CMD_CONFIGURE_STREAM=board_BoardCommand_ConfigureStream,
    WHAD_BOARD_CMD_STOP_STREAM=board_BoardCommand_StopStream,
    WHAD_BOARD_CMD_CALIBRATE=board_BoardCommand_Calibrate,
    WHAD_BOARD_CMD_GET_CALIBRATION=board_BoardCommand_GetCalibration,
    WHAD_BOARD_CMD_SET_OUTPUT=board_BoardCommand_SetOutput,
    WHAD_BOARD_CMD_GET_INPUT_STATE=board_BoardCommand_GetInputState,
    WHAD_BOARD_CMD_CONFIGURE_INPUT=board_BoardCommand_ConfigureInput,
    WHAD_BOARD_CMD_I2C_TRANSFER=board_BoardCommand_I2cTransfer,
    WHAD_BOARD_CMD_GPIO_CONFIGURE=board_BoardCommand_GpioConfigure,
    WHAD_BOARD_CMD_GPIO_READ=board_BoardCommand_GpioRead,
    WHAD_BOARD_CMD_GPIO_WRITE=board_BoardCommand_GpioWrite,
    WHAD_BOARD_CMD_ADC_READ=board_BoardCommand_AdcRead,
    WHAD_BOARD_CMD_SPI_TRANSFER=board_BoardCommand_SpiTransfer,
    WHAD_BOARD_CMD_STORAGE_INFO=board_BoardCommand_StorageInfo,
    WHAD_BOARD_CMD_STORAGE_ADOPT=board_BoardCommand_StorageAdopt,
    WHAD_BOARD_CMD_STORAGE_READ_LOG=board_BoardCommand_StorageReadLog,
    WHAD_BOARD_CMD_STORAGE_ERASE_LOG=board_BoardCommand_StorageEraseLog,
    WHAD_BOARD_CMD_GET_RUNTIME_CONFIG=board_BoardCommand_GetRuntimeConfig,
    WHAD_BOARD_CMD_SET_RUNTIME_CONFIG=board_BoardCommand_SetRuntimeConfig,
    WHAD_BOARD_CMD_SET_RUNTIME_MODE=board_BoardCommand_SetRuntimeMode,
    WHAD_BOARD_CMD_REMOTE_PROFILE_GET=board_BoardCommand_RemoteProfileGet,
    WHAD_BOARD_CMD_REMOTE_PROFILE_SET=board_BoardCommand_RemoteProfileSet,
    WHAD_BOARD_CMD_AUDIO_CONFIGURE=board_BoardCommand_AudioConfigure,
    WHAD_BOARD_CMD_RELEASE_PIN=board_BoardCommand_ReleasePin,
    WHAD_BOARD_CMD_RAW_PCM_DIAGNOSTICS=board_BoardCommand_RawPcmDiagnostics
} whad_board_command_t;

whad_board_msgtype_t whad_board_get_message_type(Message *p_message);
whad_result_t whad_board_command_from_message_type(whad_board_msgtype_t msg_type, board_BoardCommand *p_command);

#define WHAD_BOARD_MESSAGE_LIST(X) \
    X(get_board_info, board_Message_get_board_info_tag, get_board_info, board_GetBoardInfoRequest) \
    X(list_sensors, board_Message_list_sensors_tag, list_sensors, board_ListSensorsRequest) \
    X(read_sensor, board_Message_read_sensor_tag, read_sensor, board_ReadSensorRequest) \
    X(configure_stream, board_Message_configure_stream_tag, configure_stream, board_ConfigureStreamRequest) \
    X(stop_stream, board_Message_stop_stream_tag, stop_stream, board_StopStreamRequest) \
    X(calibrate, board_Message_calibrate_tag, calibrate, board_CalibrateRequest) \
    X(get_calibration, board_Message_get_calibration_tag, get_calibration, board_GetCalibrationRequest) \
    X(set_output, board_Message_set_output_tag, set_output, board_SetOutputRequest) \
    X(get_input_state, board_Message_get_input_state_tag, get_input_state, board_GetInputStateRequest) \
    X(configure_input, board_Message_configure_input_tag, configure_input, board_ConfigureInputRequest) \
    X(i2c_transfer, board_Message_i2c_transfer_tag, i2c_transfer, board_I2cTransferRequest) \
    X(gpio_configure, board_Message_gpio_configure_tag, gpio_configure, board_GpioConfigureRequest) \
    X(gpio_read, board_Message_gpio_read_tag, gpio_read, board_GpioReadRequest) \
    X(gpio_write, board_Message_gpio_write_tag, gpio_write, board_GpioWriteRequest) \
    X(adc_read, board_Message_adc_read_tag, adc_read, board_AdcReadRequest) \
    X(spi_transfer, board_Message_spi_transfer_tag, spi_transfer, board_SpiTransferRequest) \
    X(storage_info, board_Message_storage_info_tag, storage_info, board_StorageInfoRequest) \
    X(storage_adopt, board_Message_storage_adopt_tag, storage_adopt, board_StorageAdoptRequest) \
    X(storage_read_log, board_Message_storage_read_log_tag, storage_read_log, board_StorageReadLogRequest) \
    X(storage_erase_log, board_Message_storage_erase_log_tag, storage_erase_log, board_StorageEraseLogRequest) \
    X(get_runtime_config, board_Message_get_runtime_config_tag, get_runtime_config, board_GetRuntimeConfigRequest) \
    X(set_runtime_config, board_Message_set_runtime_config_tag, set_runtime_config, board_SetRuntimeConfigRequest) \
    X(set_runtime_mode, board_Message_set_runtime_mode_tag, set_runtime_mode, board_SetRuntimeModeRequest) \
    X(remote_profile_get, board_Message_remote_profile_get_tag, remote_profile_get, board_RemoteProfileGetRequest) \
    X(remote_profile_set, board_Message_remote_profile_set_tag, remote_profile_set, board_RemoteProfileSetRequest) \
    X(audio_configure, board_Message_audio_configure_tag, audio_configure, board_AudioConfigureRequest) \
    X(release_pin, board_Message_release_pin_tag, release_pin, board_ReleasePinRequest) \
    X(raw_pcm_diagnostics, board_Message_raw_pcm_diagnostics_tag, raw_pcm_diagnostics, board_RawPcmDiagnosticsRequest) \
    X(command_result, board_Message_command_result_tag, command_result, board_CommandResult) \
    X(board_info, board_Message_board_info_tag, board_info, board_GetBoardInfoResponse) \
    X(sensor_descriptor, board_Message_sensor_descriptor_tag, sensor_descriptor, board_ListSensorsResponse) \
    X(sensor_sample, board_Message_sensor_sample_tag, sensor_sample, board_SensorSample) \
    X(stream_configured, board_Message_stream_configured_tag, stream_configured, board_ConfigureStreamResponse) \
    X(calibration, board_Message_calibration_tag, calibration, board_CalibrationResponse) \
    X(input_state, board_Message_input_state_tag, input_state, board_InputStateResponse) \
    X(i2c_result, board_Message_i2c_result_tag, i2c_result, board_I2cTransferResponse) \
    X(gpio_configured, board_Message_gpio_configured_tag, gpio_configured, board_GpioConfigureResponse) \
    X(gpio_value, board_Message_gpio_value_tag, gpio_value, board_GpioReadResponse) \
    X(adc_value, board_Message_adc_value_tag, adc_value, board_AdcReadResponse) \
    X(spi_result, board_Message_spi_result_tag, spi_result, board_SpiTransferResponse) \
    X(storage_status, board_Message_storage_status_tag, storage_status, board_StorageInfoResponse) \
    X(runtime_config, board_Message_runtime_config_tag, runtime_config, board_RuntimeConfigResponse) \
    X(remote_profile, board_Message_remote_profile_tag, remote_profile, board_RemoteProfileResponse) \
    X(audio_configured, board_Message_audio_configured_tag, audio_configured, board_AudioConfigureResponse) \
    X(audio_chunk, board_Message_audio_chunk_tag, audio_chunk, board_AudioChunk) \
    X(input_event, board_Message_input_event_tag, input_event, board_InputEvent) \
    X(gesture_event, board_Message_gesture_event_tag, gesture_event, board_GestureEvent) \
    X(log_chunk, board_Message_log_chunk_tag, log_chunk, board_LogChunk) \
    X(board_status, board_Message_board_status_tag, board_status, board_BoardStatus)

#define WHAD_BOARD_DECLARE_MESSAGE(name, tag, field, type) \
    whad_result_t whad_board_##name(Message *p_message, uint32_t request_id, const type *p_value); \
    whad_result_t whad_board_##name##_parse(Message *p_message, uint32_t *p_request_id, type *p_value);

WHAD_BOARD_MESSAGE_LIST(WHAD_BOARD_DECLARE_MESSAGE)

#undef WHAD_BOARD_DECLARE_MESSAGE

#ifdef __cplusplus
}
#endif

#endif
