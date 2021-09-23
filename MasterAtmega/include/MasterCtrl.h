//
// File: MasterModel.h
//
// Code generated for Simulink model 'MasterModel'.
//
// Model version                  : 1.4
// Simulink Coder version         : 9.1 (R2019a) 23-Nov-2018
// C/C++ source code generated on : Mon Jan 24 18:32:35 2020
//
// Target selection: ert.tlc
// Embedded hardware selection: Atmel->AVR
// Code generation objectives:
//    1. Execution efficiency
//    2. RAM efficiency
// Validation result: Not run
//
#ifndef RTW_HEADER_MasterCtrl_h_
#define RTW_HEADER_MasterCtrl_h_
#ifndef MasterCtrl_COMMON_INCLUDES_
#define MasterCtrl_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif // MasterCtrl_COMMON_INCLUDES_

// Macros for accessing real-time model data structure
#ifndef rtmGetErrorStatus
#define rtmGetErrorStatus(rtm) ((rtm)->errorStatus)
#endif

#ifndef rtmSetErrorStatus
#define rtmSetErrorStatus(rtm, val) ((rtm)->errorStatus = (val))
#endif

// Forward declaration for rtModel
typedef struct tag_RTM RT_MODEL;

// Block signals and states (default storage) for system '<Root>'
typedef struct
{
  uint8_T is_active_c3_MasterCtrl; // '<Root>/MasterModel'
  uint8_T is_c3_MasterCtrl;        // '<Root>/MasterModel'
  uint8_T is_Read;                 // '<Root>/MasterModel'
  uint8_T is_Write;                // '<Root>/MasterModel'
  boolean_T wrt2Bus;               // '<Root>/MasterModel'
} DW;

// Real-time Model Data Structure
struct tag_RTM
{
  const char_T *volatile errorStatus;
};

// Class declaration for model MasterModel
class MasterModel
{
  // public data and function members
public:
  // model initialize function
  void initialize();

  // model step function
  void step();

  // Constructor
  MasterModel();

  // Destructor
  ~MasterModel();

  // Real-Time Model get method
  RT_MODEL *getRTM();

  // private data and function members
private:
  // Block signals and states
  DW rtDW;

  // Real-Time Model
  RT_MODEL rtM;
};

//-
//  The generated code includes comments that allow you to trace directly
//  back to the appropriate location in the model.  The basic format
//  is <system>/block_name, where system is the system number (uniquely
//  assigned by Simulink) and block_name is the name of the block.
//
//  Use the MATLAB hilite_system command to trace the generated code back
//  to the model.  For example,
//
//  hilite_system('<S3>')    - opens system 3
//  hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
//
//  Here is the system hierarchy for this model
//
//  '<Root>' : 'MasterModel'
//  '<S1>'   : 'MasterModel/MasterModel'

#endif // RTW_HEADER_MasterCtrl_h_

//
// File trailer for generated code.
//
// [EOF]
//
