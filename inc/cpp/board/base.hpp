#ifndef __INC_WHAD_BOARD_HPP
#define __INC_WHAD_BOARD_HPP

#include "message.hpp"
#include "common.hpp"
#include "../domains/board.h"

namespace whad
{
    namespace board
    {
        enum MessageType {
            UnknownMsg = WHAD_BOARD_UNKNOWN,
            GetBoardInfoMsg = WHAD_BOARD_GET_BOARD_INFO,
            ListSensorsMsg = WHAD_BOARD_LIST_SENSORS,
            ReadSensorMsg = WHAD_BOARD_READ_SENSOR,
            ConfigureStreamMsg = WHAD_BOARD_CONFIGURE_STREAM,
            StopStreamMsg = WHAD_BOARD_STOP_STREAM,
            CalibrateMsg = WHAD_BOARD_CALIBRATE,
            GetCalibrationMsg = WHAD_BOARD_GET_CALIBRATION,
            SetOutputMsg = WHAD_BOARD_SET_OUTPUT,
            GetInputStateMsg = WHAD_BOARD_GET_INPUT_STATE,
            ConfigureInputMsg = WHAD_BOARD_CONFIGURE_INPUT,
            I2cTransferMsg = WHAD_BOARD_I2C_TRANSFER,
            GpioConfigureMsg = WHAD_BOARD_GPIO_CONFIGURE,
            GpioReadMsg = WHAD_BOARD_GPIO_READ,
            GpioWriteMsg = WHAD_BOARD_GPIO_WRITE,
            AdcReadMsg = WHAD_BOARD_ADC_READ,
            SpiTransferMsg = WHAD_BOARD_SPI_TRANSFER,
            StorageInfoMsg = WHAD_BOARD_STORAGE_INFO,
            StorageAdoptMsg = WHAD_BOARD_STORAGE_ADOPT,
            StorageReadLogMsg = WHAD_BOARD_STORAGE_READ_LOG,
            StorageEraseLogMsg = WHAD_BOARD_STORAGE_ERASE_LOG,
            GetRuntimeConfigMsg = WHAD_BOARD_GET_RUNTIME_CONFIG,
            SetRuntimeConfigMsg = WHAD_BOARD_SET_RUNTIME_CONFIG,
            SetRuntimeModeMsg = WHAD_BOARD_SET_RUNTIME_MODE,
            RemoteProfileGetMsg = WHAD_BOARD_REMOTE_PROFILE_GET,
            RemoteProfileSetMsg = WHAD_BOARD_REMOTE_PROFILE_SET,
            AudioConfigureMsg = WHAD_BOARD_AUDIO_CONFIGURE,
            ReleasePinMsg = WHAD_BOARD_RELEASE_PIN,
            RawPcmDiagnosticsMsg = WHAD_BOARD_RAW_PCM_DIAGNOSTICS,
            CommandResultMsg = WHAD_BOARD_COMMAND_RESULT,
            BoardInfoMsg = WHAD_BOARD_INFO,
            SensorDescriptorMsg = WHAD_BOARD_SENSOR_DESCRIPTOR,
            SensorSampleMsg = WHAD_BOARD_SENSOR_SAMPLE,
            StreamConfiguredMsg = WHAD_BOARD_STREAM_CONFIGURED,
            CalibrationMsg = WHAD_BOARD_CALIBRATION,
            InputStateMsg = WHAD_BOARD_INPUT_STATE,
            I2cResultMsg = WHAD_BOARD_I2C_RESULT,
            GpioConfiguredMsg = WHAD_BOARD_GPIO_CONFIGURED,
            GpioValueMsg = WHAD_BOARD_GPIO_VALUE,
            AdcValueMsg = WHAD_BOARD_ADC_VALUE,
            SpiResultMsg = WHAD_BOARD_SPI_RESULT,
            StorageStatusMsg = WHAD_BOARD_STORAGE_STATUS,
            RuntimeConfigMsg = WHAD_BOARD_RUNTIME_CONFIG,
            RemoteProfileMsg = WHAD_BOARD_REMOTE_PROFILE,
            AudioConfiguredMsg = WHAD_BOARD_AUDIO_CONFIGURED,
            AudioChunkMsg = WHAD_BOARD_AUDIO_CHUNK,
            InputEventMsg = WHAD_BOARD_INPUT_EVENT,
            GestureEventMsg = WHAD_BOARD_GESTURE_EVENT,
            LogChunkMsg = WHAD_BOARD_LOG_CHUNK,
            BoardStatusMsg = WHAD_BOARD_STATUS
        };

        class BoardMsg : public NanoPbMsg
        {
            public:
                BoardMsg();
                BoardMsg(NanoPbMsg &pMessage);
                ~BoardMsg();

                MessageType getType(void);
        };
    }
}

#endif
