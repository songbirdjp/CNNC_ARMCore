/************************************************************************************/
// 01-3F: Calibration Commands              ————>   DOSE_CalibrationCommands_t
// 40-7F: Treatment Parameters Commands     ————>   TreatmentParametersCommands_t
// 80-BF: Interlock Commands                ————>   InterlockCommands_t
// C0-FF: Status and Control Commands       ————>   StatusControlCommands_t

/**************************************RS422 with Dose board**********************************************/

/*frame type */














/*==========================For 1025，Ethercat=========================*/
/* ======================== 01-3F: Calibration Commands ======================== */

/* 0x01: Calibration parameter unlock and validity */
typedef struct
{
    uint8_t unlock_0x00; // 0x00: Prohibit modification (default), 1: Allow modification
    uint8_t valid_0x01;  // 0x01: Calibration parameter valid (0: Invalid, 1: Valid)
} DOSE_CalibrationParam_0x01_t;

/* 0x02: ADC code value setting */
typedef struct
{
    uint32_t adc_value_KADC_0x00;           // 0x00: 1MU corresponding ADC value, 3 bytes
    uint32_t adc_value_backup_0x01_0x04[4]; // 0x01-0x04: 1MU corresponding ADC value backup for specific PRF, 3 bytes each
} DOSE_ADCCalibrationParam_0x02_t;

/* 标定命令容器 */
typedef struct
{
    /* 0x01: Calibration parameter unlock and validity */
    DOSE_CalibrationParam_0x01_t calibrationParam;
    /* 0x02: ADC code value setting */
    DOSE_ADCCalibrationParam_0x02_t adcCalibrationParam;
    /* 0x03: DummyDose DAC ratio coefficient */
    uint16_t DOSE_dac_ratio_coefficient_KDAC_0x0300; // 0x00: default = 30, DAC output = KADC * KDAC * dose rate set / 6E7
    /* 0x04: Minimum trigger interval */
    uint16_t DOSE_min_trigger_interval_us_0x0400; // 0x00: in microseconds, default = 4000
} DOSE_CalibrationCommands_t;

/* ======================== 40-7F: Treatment Parameters Commands ======================== */

/* 0x41: Pulse output mode */
typedef struct
{
    uint8_t pulse_output_mode_0x00;    // 0x00: 0: Fixed PRF (default), 1: Fixed dose rate
    uint32_t fixed_PRF_frequency_0x01; // 0x01: Fixed PRF frequency in Hz, default = minimum trigger time
} DOSE_PulseOutputMode_0x41_t;

/* 0x42 RadiationIndexItems*/
typedef struct
{
    uint8_t control_point_number_0x03; // 0x03: Control point number
    uint16_t tolerance_value_0x03;     // 0x03: Tolerance value (unit: 0.1MU or percentage)
} ControlPointTolerance_t;

typedef struct
{
    uint8_t control_point_number_0x04;    // 0x04: Control point number
    uint16_t radiation_point_number_0x04; // 0x04: Radiation point number
} ControlPointRadiationMapping_t;

typedef struct
{
    uint16_t radiation_point_number_0x05; // 0x05: Radiation point number
    uint16_t total_dose_0x01MU_0x05;      // 0x05: Total dose for the point in 0.1MU units
    uint16_t expected_time_ms_0x05;       // 0x05: Expected time for the point in milliseconds
} SingleRadiationPoint_t;

typedef struct
{
    uint8_t source_0x4200;                         // 0x00: Passive (from BGM ARM IO), 1: Active (generated locally)
    uint16_t total_dose_0x01MU_0x4201;             // 0x01: Total dose set in 0.1MU units
    uint16_t control_radiation_point_count_0x4202; // 0x02: Control point total count, Radiation point total count
    ControlPointTolerance_t control_point_tolerance_0x03;
    ControlPointRadiationMapping_t control_point_radiation_mapping_0x04;
    SingleRadiationPoint_t single_radiation_point_0x05;
} DOSE_RadiationIndexItem_0x42_t;

/* 0x43: Beam data lock/unlock */
typedef struct
{
    uint8_t unlock_0x00; // 0x00: Unlock the beam data
    uint8_t lock_0x01;   // 0x01: Lock the beam data after setting completion
} DOSE_BeamDataLock_0x43_t;

/* 治疗参数命令容器 */
typedef struct
{
    /* 0x40: Dose generation mode */
    uint8_t DOSE_DoseGenerationMode_0x4000;
    /* 0x41: Pulse output mode */
    DOSE_PulseOutputMode_0x41_t pulseOutputMode;
    /* 0x42: Radiation Index source */
    DOSE_RadiationIndexItem_0x42_t radiationIndexItem;
    /* 0x43: Beam data lock/unlock */
    DOSE_BeamDataLock_0x43_t beamDataLock;
} TreatmentParametersCommands_t;

/* ======================== 80-BF: Interlock Commands ======================== */

/* 0x80: Dose rate interlock threshold */
typedef struct
{
    uint8_t low_threshold_0x00;  // 0x00: Dose rate low interlock threshold (%)
    uint8_t high_threshold_0x01; // 0x01: Dose rate high interlock threshold (%)
} DoseRateInterlockThreshold_0x80_t;

/* 0x80: Control point dose interlock threshold */
typedef struct
{
    uint8_t total_dose_low_threshold_0x02;  // 0x02: Total dose low interlock threshold (%)
    uint8_t total_dose_high_threshold_0x03; // 0x03: Total dose high interlock threshold (%)
} ControlPointDoseInterlockThreshold_0x80_t;

/* 0x80: Symmetry threshold and pulse count thresholds */
typedef struct
{
    uint8_t symmetry_threshold_0x04;     // 0x04: Symmetry threshold (%)
    uint8_t max_pulse_count_0x05;        // 0x05: Maximum pulse count threshold (%)
    uint8_t min_pulse_count_0x06;        // 0x06: Minimum pulse count threshold (%)
    uint16_t communication_timeout_0x07; // 0x07: Communication timeout in milliseconds
} SymmetryAndPulseThresholds_0x80_t;

/* 0x81: Pulse count queries */
typedef struct
{
    uint8_t over_large_pulse_count;
    uint8_t over_small_pulse_count;
    uint8_t abnormal_pulse_count;
} PulseCountQuery_0x81_t;

/* 0x82: System feedback queries */
typedef struct
{
    uint8_t ion_chamber_bias_feedback;
    uint8_t P5V_power_feedback;
    uint8_t N5V_power_feedback;
    uint16_t DAC1_Offset_feedback;
    uint16_t DAC2_Offset_feedback;
} SystemFeedback_0x82_t;

/* 0xB1: Flash fault handling commands */
typedef struct
{
    uint8_t query_flash_fault_0x00; // 0x00: Get flash fault record
    uint8_t clear_flash_fault_0x01; // 0x01: Clear flash fault record
} FlashFaultCmd_0xB1_t;

/* 联锁命令容器 */
typedef struct
{
    DoseRateInterlockThreshold_0x80_t doseRateInterlockThreshold;
    ControlPointDoseInterlockThreshold_0x80_t controlPointDoseInterlockThreshold;
    SymmetryAndPulseThresholds_0x80_t symmetryAndPulseThresholds;
    PulseCountQuery_0x81_t pulseCountQuery;
    SystemFeedback_0x82_t systemFeedback;
    /* 0xB0: Interlock query command */
    uint8_t interlock_code_0xB000;
    FlashFaultCmd_0xB1_t flashFaultCmd;
} InterlockCommands_t;

/* ======================== C0-FF: Status and Control Commands ======================== */

/* 0xC0: Start various operations */
typedef struct {
    uint8_t dummy_dose_start_0x00;  
    // 0x00: Start DummyDose, 0x01: Start READY, 0x02: Start Radiation, 0x03: Jump to SETTING
    uint8_t ready_start_0x01;
    uint8_t radiation_start_0x02;
    uint8_t setting_start_0x03;
} StartOperationCmd_0xC0_t;
/* 0xFA: Clear specific counters and flags */
typedef struct {
    uint8_t clear_pulse_count_0x00;   // 0x00: Clear abnormal pulse count
    uint8_t clear_patient_data_0x01;  // 0x01: Clear patient data
    uint8_t clear_dose_counter_0x02;  // 0x02: Clear dose counter
    uint8_t clear_interlock_0x03;     // 0x03: Clear interlock
    uint8_t clear_dose_flag_0x04;     // 0x04: Clear single pulse dose valid flag
} ClearCmd_0xFA_t;

/* 0xFB: Reset system or watchdog */
typedef struct {
    uint8_t reset_watchdog_0x00;  // 0x00: Reset the watchdog
    uint8_t global_reset_0x01;    // 0x01: Perform a global reset
} SystemResetCmd_0xFB_t;

/* 状态和控制命令容器 */
typedef struct {
    StartOperationCmd_0xC0_t startOperationCmd;
    /* 0xCF: Query current system status */
    uint8_t query_status_0xCF00;  // 0x00: Query current status
    ClearCmd_0xFA_t clearCmd;
    SystemResetCmd_0xFB_t systemResetCmd;
} StatusControlCommands_t;

// typedef struct {
   
  
// } DOSE_P2A_ECAT_DATA_t;

// typedef struct {
   
  
// } DOSE_A2P_ECAT_DATA_t;
#endif /* DOSE_COMMANDS_H */
