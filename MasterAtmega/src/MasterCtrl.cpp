//
// File: MasterCtrl.cpp
//
// Code generated for Simulink model 'MasterCtrl'.
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
#include "MasterCtrl.h"

// Named constants for Chart: '<Root>/MasterCtrl'
#define IN_Data ((uint8_T)1U)
#define IN_Device_Address ((uint8_T)2U)
#define IN_Dip_1 ((uint8_T)1U)
#define IN_Dip_2 ((uint8_T)2U)
#define IN_NO_ACTIVE_CHILD ((uint8_T)0U)
#define IN_Read ((uint8_T)1U)
#define IN_Read_idle ((uint8_T)3U)
#define IN_Register_Address ((uint8_T)3U)
#define IN_Write ((uint8_T)2U)
#define IN_Write_start ((uint8_T)4U)

// Model step function
void MasterModel::step()
{
  // Chart: '<Root>/MasterCtrl'
  if (rtDW.is_active_c3_MasterCtrl == 0U)
  {
    rtDW.is_active_c3_MasterCtrl = 1U;
    rtDW.is_c3_MasterCtrl = IN_Read;
    rtDW.is_Read = IN_Read_idle;
  }
  else if (rtDW.is_c3_MasterCtrl == IN_Read)
  {
    if (rtDW.wrt2Bus)
    {
      rtDW.is_Read = IN_NO_ACTIVE_CHILD;
      rtDW.is_c3_MasterCtrl = IN_Write;
      rtDW.is_Write = IN_Write_start;
    }
    else
    {
      switch (rtDW.is_Read)
      {
      case IN_Dip_1:
        rtDW.wrt2Bus = true;
        break;

      case IN_Dip_2:
        rtDW.wrt2Bus = true;
        break;

      default:
        // case IN_Read_idle:
        break;
      }
    }
  }
  else
  {
    // case IN_Write:
    if (!rtDW.wrt2Bus)
    {
      rtDW.is_Write = IN_NO_ACTIVE_CHILD;
      rtDW.is_c3_MasterCtrl = IN_Read;
      rtDW.is_Read = IN_Read_idle;
    }
    else
    {
      switch (rtDW.is_Write)
      {
      case IN_Data:
        rtDW.wrt2Bus = false;
        break;

      case IN_Device_Address:
        rtDW.is_Write = IN_Register_Address;
        break;

      case IN_Register_Address:
        rtDW.is_Write = IN_Data;
        rtDW.wrt2Bus = false;
        break;

      default:
        // case IN_Write_start:
        rtDW.is_Write = IN_Device_Address;
        break;
      }
    }
  }

  // End of Chart: '<Root>/MasterCtrl'
}

// Model initialize function
void MasterModel::initialize()
{
  // (no initialization code required)
}

// Constructor
MasterModel::MasterModel()
{
  // Currently there is no constructor body generated.
}

// Destructor
MasterModel::~MasterModel()
{
  // Currently there is no destructor body generated.
}

// Real-Time Model get method
RT_MODEL *MasterModel::getRTM()
{
  return (&rtM);
}

//
// File trailer for generated code.
//
// [EOF]
//
