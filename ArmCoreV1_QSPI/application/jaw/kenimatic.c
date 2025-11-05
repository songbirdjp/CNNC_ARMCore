#include "kenimatic.h"

//PID calculation

//static uint32_t location_timer = 0;

int8_t SVG(struct SVG_Type* inst, uint8_t axes);
void initSVG(struct SVG_Type* inst, uint8_t axes);

float PID_Compute(PID_TypeDef *pid, float current, float setpoint)
{
    pid->Setpoint = setpoint;
    float error = pid->Setpoint - current;
    if((error >= -pid->deadZone)&&(error <= pid->deadZone))
    {
        error = 0;
        pid->Integral = 0;
        pid->PreviousError = 0;
    }
    if((error >= -pid->integralStartErr)&&(error <= pid->integralStartErr)) {
        pid->Integral += error;
        if (pid->Integral > pid->IntegralLimit)
            pid->Integral = pid->IntegralLimit;
        if (pid->Integral < -pid->IntegralLimit)
            pid->Integral = -pid->IntegralLimit;
    }

    float derivative = error - (pid->PreviousError);
    float output = (pid->Kp * error) + (pid->Ki * pid->Integral) + (pid->Kd * derivative);
    pid->PreviousError = error;
    if (output > pid->OutputLimit)
    {
        output = pid->OutputLimit;
    }
    else if (output < -pid->OutputLimit)
    {
        output = -pid->OutputLimit;
    }

    return output;
}

float PositionPIDCtrl(uint32_t current_position, uint32_t _setPosition, PID_TypeDef *PID_parameters)
{
    float _pidPositionOutput;
    //float current_position = __HAL_TIM_GET_COUNTER(&htim5);
    _pidPositionOutput = PID_Compute(PID_parameters, current_position, _setPosition);
    //MotorYCtrlByPWM((int16_t) pid_output);
    return _pidPositionOutput;
}

float SpeedPIDCtrl(float actualSpeed, float _setSpeed, PID_TypeDef *PID_parameters)
{
    float _pidSpeedOutput = PID_Compute(PID_parameters, actualSpeed, _setSpeed);
    return  _pidSpeedOutput;
}

void initSVG(struct SVG_Type* inst, uint8_t axes)
{
    memset(inst, 0, sizeof(struct SVG_Type));

    inst->DynamicValues.AccelerationNeg = 170;//mm/s^2
    inst->DynamicValues.AccelerationPos = 170;
    if(axes){
        inst->DynamicValues.VelocityNeg = 17;//max average should = 12.88
        inst->DynamicValues.VelocityPos = 17;
    }
    else{
        inst->DynamicValues.VelocityNeg = 17;//max average should = 8.52
        inst->DynamicValues.VelocityPos = 17;
    }
    inst->DynamicValues.JerkNeg = 2400;//mm/s^3
    inst->DynamicValues.JerkPos = 2400;
    inst->DynamicLimits.AccelerationNeg = 170;
    inst->DynamicLimits.AccelerationPos = 170;
    inst->DynamicLimits.JerkNeg = 2500;
    inst->DynamicLimits.JerkPos = 2500;
    if(axes){   //Y Jaw 
        inst->DynamicLimits.PositionNeg = 0.0;
        inst->DynamicLimits.PositionPos = 1000;
    }    
    else{   //X Jaw 
        inst->DynamicLimits.PositionNeg = 0.0;
        inst->DynamicLimits.PositionPos = 1000;
    }
    inst->DynamicLimits.VelocityNeg = 17.5;
    inst->DynamicLimits.VelocityPos = 17.5;
    inst->StartPosition = 0;//ENC: 2000
    inst->TargetPosition = 0;
    inst->Cycletime = 0.004;//s
    inst->Override = 1;
    if (inst->Cycletime <= 0)
    {
        inst->Cycletime = 0;
        inst->Status = ERROR_SVG_CYCLETIME;
    }
    if (inst->Override < 0)
    {
        inst->Override = 0;
        inst->Status = ERROR_SVG_OVERRIDE;
    }
    inst->Start = 0;
    inst->Enable = 0;
    inst->State = STATE_STANDSTILL;
}

#define max(a,b) ( (a>b)?a:b )

unsigned short MaxMovementDynamics(double d, double a, double j, double v, double v_start, double v_end, double *v_max, double *a_start_max, double *a_end_max, double *d_linear)
{ //find maximum speed and acceleration of a movement with length d, speed v, acceleration a, jerk j, initial and final speed v_start and v_end
    //also returns size of linear speed interval

    double d_start, d_end;
    double t_start, t_end;
    double a_start, a_end;
    double dv_start, dv_end;

    double step, Delta, v_high, v_low;

    if ((d < 0)  /*|| (v_end < 0) || (v_end > v) || (v_start < 0) || (v_start > v) */ || (a <= 0) || (j <= 0))
    {// incorrect input -> error
        return 255;
    }

    if (d==0)
    { //zero length -> no movement possible
        if (v_start!=v_end)
        {
            return 255;
        }
        else
        {
            *v_max = v_start;
            *a_start_max = 0;
            *a_end_max = 0;
            *d_linear = 0;
            return 0;
        }
    }

    if (v_start > v)    v_start = v;
    else if (v_start < 0)   v_start = 0;

    if (v_end > v)    v_end = v;
    else if (v_end < 0)   v_end = 0;
    /* assume that v can be reached */

    // interval from v_start to v
    a_start = sqrt(j*(v-v_start));
    if (a_start > a) a_start = a;
    dv_start = v - v_start;
    if (a_start != 0)
    {
        t_start = dv_start/a_start + a_start/j;
        d_start = 0.5 * (dv_start*dv_start/a_start + dv_start*a_start/j) + v_start * t_start;
    }
    else
    {
        t_start = 0;
        d_start = 0;
    }

    // interval from v to v_end
    a_end = sqrt(j*(v-v_end));
    if (a_end > a) a_end = a;
    dv_end = v - v_end;
    if (a_end != 0)
    {
        t_end = dv_end/a_end + a_end/j;
        d_end = 0.5 * (dv_end*dv_end/a_end + dv_end*a_end/j) + v_end * t_end;
    }
    else
    {
        t_end = 0;
        d_end = 0;
    }

    Delta = d - d_start - d_end; //constant speed section

    if (Delta >= 0)
    {// v can be reached
        *v_max = v;
        *a_start_max = a_start;
        *a_end_max = a_end;
        *d_linear = Delta;
        return 0;
    }
    else
    {// v cannot be reached, reduce v
        step = 0;
        v_high = v;
        v_low = max(v_start,v_end);
        do
        {
            step += 0.01;

            if (Delta < 0)
            {
                v_high = v;
                v = (v_high + v_low)/2.0;
            }
            else if (Delta > 0)
            {
                v_low = v;
                v = (v_high + v_low)/2.0;
            }
            else if (Delta == 0)
            {
                step = 1;
            }

            // interval from v_start to v
            a_start = sqrt(j*(v-v_start));
            if (a_start > a) a_start = a;
            dv_start = v - v_start;
            if (a_start != 0)
            {
                t_start = dv_start/a_start + a_start/j;
                d_start = 0.5 * (dv_start*dv_start/a_start + dv_start*a_start/j) + v_start * t_start;
            }
            else
            {
                t_start = 0;
                d_start = 0;
            }

            // interval from v to v_end
            a_end = sqrt(j*(v-v_end));
            if (a_end > a) a_end = a;
            dv_end = v - v_end;
            if (a_end != 0)
            {
                t_end = dv_end/a_end + a_end/j;
                d_end = 0.5 * (dv_end*dv_end/a_end + dv_end*a_end/j) + v_end * t_end;
            }
            else
            {
                t_end = 0;
                d_end = 0;
            }

            Delta = d - d_start - d_end;

        } while (step < 1);

        *v_max = v;
        *a_start_max = a_start;
        *a_end_max = a_end;
        *d_linear = 0; // no linear speed interval
        return 0;
    }
}

static SVG_Motion_Param motionParam[XY] = {0};
int8_t SVG(struct SVG_Type* inst, uint8_t axes)
{

    if (!inst->Enable)
    {
        inst->Status = ERROR_DISABLED;
        inst->State = STATE_STANDSTILL;
        inst->Start = 0;
        inst->Stop = 0;
        inst->EStop = 0;
        return -1;
    }
    else if (inst->Status == ERROR_DISABLED)
    {// reset status at enable positive edge
        inst->Status = STATUS_OK;
    }

    double oldPosition, oldSpeed;
    double v, a, j, t, d;
    double v1, v2, v3, v4, v5, v6, v7;
  //  static double v_max, a_start_max, a_end_max;
  //  static double T1, T2, T3, T4, T5, T6, T7;  
  //  static double v_start = 0, v_end = 0;
    oldPosition = inst->Position;
    oldSpeed = inst->Speed;

    /*************** simplify variable notations ***************/
    v = inst->v;
    a = inst->a;
    j = inst->j;
    double cycletime = inst->Cycletime;
//    if (cycletime <= 0)
//    {
//        cycletime = 0;
//        inst->Status = ERROR_SVG_CYCLETIME;
//    }

    double override = inst->Override;
//    if (override < 0)
//    {
//        override = 0;
//        inst->Status = ERROR_SVG_OVERRIDE;
//    }

    /*************** evaluate INPUT commands ***************/

    if ((inst->Start)/* && (inst->State == STATE_STANDSTILL)*/) //currently only works from standstill
    {
        inst->Start = 0;
        inst->Status = STATUS_OK;

        /* new movement? -> set start position */
        if (inst->State == STATE_STANDSTILL)
        {
            inst->Position = inst->StartPosition;
            inst->beginPosition = inst->StartPosition;
            oldPosition = inst->Position;
            motionParam[axes].v_start = inst->Speed;
            motionParam[axes].v_end = 0;
        }
        else
        {// start from current position
            inst->beginPosition = inst->Position;
            motionParam[axes].v_start = inst->Speed;
            motionParam[axes].v_end = 0;
        }

        /* determine moving direction */
        inst->moveDirection = sign(inst->TargetPosition - inst->Position);

        /* simplify variable notations */
        if (inst->moveDirection >0)
        { //positive move
            v = inst->DynamicValues.VelocityPos;
            a = inst->DynamicValues.AccelerationPos;
            j = inst->DynamicValues.JerkPos;
            //check that given movement parameters are within dynamic limits
            if ((v<=0)||(v>inst->DynamicLimits.VelocityPos))
            {
                inst->Status = ERROR_SVG_LIMIT_VEL;
                return -1;
            }
            if ((a<=0)||(a>inst->DynamicLimits.AccelerationPos))
            {
                inst->Status = ERROR_SVG_LIMIT_ACC;
                return -1;
            }
            if ((j<=0)||(j>inst->DynamicLimits.JerkPos))
            {
                inst->Status = ERROR_SVG_LIMIT_JERK;
                return -1;
            }
        }
        else
        { //negative move
            v = inst->DynamicValues.VelocityNeg;
            a = inst->DynamicValues.AccelerationNeg;
            j = inst->DynamicValues.JerkNeg;
            //check that given movement parameters are within dynamic limits
            if ((v<=0)||(v>inst->DynamicLimits.VelocityNeg))
            {
                inst->Status = ERROR_SVG_LIMIT_VEL;
                return -1;
            }
            if ((a<=0)||(a>inst->DynamicLimits.AccelerationNeg))
            {
                inst->Status = ERROR_SVG_LIMIT_ACC;
                return -1;
            }
            if ((j<=0)||(j>inst->DynamicLimits.JerkNeg))
            {
                inst->Status = ERROR_SVG_LIMIT_JERK;
                return -1;
            }
        }

        /* check static limits for start position */
        if (inst->DynamicLimits.PositionNeg >= inst->DynamicLimits.PositionPos)
        {
            inst->Status = ERROR_SVG_LIMIT_POS;
            return -1;
        }
        if ((inst->beginPosition > inst->DynamicLimits.PositionPos)||(inst->beginPosition < inst->DynamicLimits.PositionNeg))
        {
            inst->Status = ERROR_SVG_LIMIT_POS;	//start position error
            return -1;
        }

        /* check static limits for target position */
        if (inst->TargetPosition > inst->DynamicLimits.PositionPos)
        {
            inst->endPosition = inst->DynamicLimits.PositionPos;
            inst->endLimits = 1;
            inst->Status = ERROR_SVG_LIMIT_POS_POS;
            return -1;
        }
        else if (inst->TargetPosition < inst->DynamicLimits.PositionNeg)
        {
            inst->endPosition = inst->DynamicLimits.PositionNeg;
            inst->endLimits = 1;
            inst->Status = ERROR_SVG_LIMIT_POS_NEG;
            return -1;
        }
        else
        {
            inst->endPosition = inst->TargetPosition;
            inst->endLimits = 0;
        }
      //  if(axes == X)   LOG_I("SVG pos: %lf %lf %lf\r\n", inst->beginPosition, inst->endPosition, inst->TargetPosition);

        /* absolute value of total movement distance */
        d = (inst->endPosition - inst->Position) * inst->moveDirection;
        motionParam[axes].v_start *= inst->moveDirection;

        if ((inst->Status == STATUS_OK)/* && (inst->State == STATE_STANDSTILL)*/) //currently only works from standstill
        { //start movement
            inst->Done = 0;
            inst->State = STATE_MOVING;
            inst->elapsedTime = 0.0;

            // numerical computation of dynamic values
            if (0xFF == MaxMovementDynamics(d, a, j, v, motionParam[axes].v_start, motionParam[axes].v_end, &motionParam[axes].v_max, 
                    &motionParam[axes].a_start_max, &motionParam[axes].a_end_max, &inst->delta))
			
            {
                inst->Status = ERROR_SVG_DYNCALC;	//start position error
                return -1;
            }
		//	if(axes == X) LOG_I("%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\r\n",d, v, a, j, motionParam[axes].v_start, motionParam[axes].v_max, motionParam[axes].a_start_max, motionParam[axes].a_end_max, inst->delta);

            //			/* calculate what kind of s-curve is needed */
            //			inst->delta = d - (v*v/a + a*v/j);	//interval with linear speed
            //			inst->epsilon = d - a*a/j * (-a/j + sqrt(a*a/j/j + 4.0*d/a));	//interval with linear acceleration (pos and neg) when delta is not positive
            //
            //			/* are max speed and acceleration reached? */
            //			if (inst->delta <= 0)
            //			{ //max speed cannot be reached -> recalculate it!
            //				inst->delta = 0;
            //				if (inst->epsilon <= 0)
            //				{//max acceleration cannot be reached -> recalculate it!
            //					a = pow((d*j*j/2.0),(1.0/3.0));
            //				}
            //				if (a!=0)
            //				{
            //					v = a/2.0*(-a/j+sqrt(a*a/j/j+4.0*d/a));
            //				}
            //				else
            //				{// d=0 no movement required
            //					v = 0;
            //				}
            //			}

            /* calculate curve time zones according to previously calculated a and v */
            if ((a!=0)&&(v!=0))
            {
               if (fabs(motionParam[axes].a_start_max) > 1e-6) {
                    inst->dt[0] = motionParam[axes].a_start_max / j;
                    inst->dt[1] = inst->dt[0] + (motionParam[axes].v_max - motionParam[axes].v_start) / motionParam[axes].a_start_max - inst->dt[0];
                }
                else {
                    inst->dt[0] = 0;
                    inst->dt[1] = 0;
                }
                inst->dt[2] = inst->dt[1] + inst->dt[0];
                inst->dt[3] = inst->dt[2] + inst->delta / motionParam[axes].v_max;
                if (fabs(motionParam[axes].a_end_max) > 1e-6) {
                    inst->dt[4] = inst->dt[3] + motionParam[axes].a_end_max / j;
                    inst->dt[5] = inst->dt[4] + (motionParam[axes].v_max - motionParam[axes].v_end) / motionParam[axes].a_end_max - motionParam[axes].a_end_max / j;
                }
                else {
                    inst->dt[4] = 0;
                    inst->dt[5] = 0;
                }
                inst->dt[6] = inst->dt[5] + motionParam[axes].a_end_max / j;
                motionParam[axes].T1 = inst->dt[0];
                motionParam[axes].T2 = inst->dt[1] - inst->dt[0];
                motionParam[axes].T3 = inst->dt[2] - inst->dt[1];
                motionParam[axes].T4 = inst->dt[3] - inst->dt[2];
                motionParam[axes].T5 = inst->dt[4] - inst->dt[3];
                motionParam[axes].T6 = inst->dt[5] - inst->dt[4];
                motionParam[axes].T7 = inst->dt[6] - inst->dt[5];
              //  if(axes == X) LOG_I("%lf,%lf,%lf,%lf,%lf,%lf,%lf\r\n", 
                  //  motionParam[axes].T1, motionParam[axes].T2, motionParam[axes].T3, motionParam[axes].T4, motionParam[axes].T5, motionParam[axes].T6, motionParam[axes].T7);
            }
            else
            {
                inst->dt[0] = 0;
                inst->dt[1] = 0;
                inst->dt[2] = 0;
                inst->dt[3] = 0;
                inst->dt[4] = 0;
                inst->dt[5] = 0;
                inst->dt[6] = 0;
            }
        }
        else if (inst->Status != STATUS_OK)
        {
            inst->State = STATE_ERROR;
            return -1;
        }
    }

    if (inst->Stop)
    {
        inst->Stop = 0;
        if ((inst->State == STATE_MOVING)&&(inst->Phase != 2)) //stop active movement that is not decelerating already
        {
            inst->State = STATE_STOPPING;
            inst->beginPosition = inst->Position;

            if (override > 0)
                inst->beginSpeed = inst->Speed / override;
            else
                inst->beginSpeed = 0;

            v = inst->beginSpeed;
            inst->beginAcc = inst->Acceleration;
            a = inst->beginAcc;
            inst->elapsedTime = 0;
            inst->dt[0] = 0;
            if(inst->Phase <= 1)
            {// movement is accelerating -> need to bring acceleration to zero first
                inst->dt[0] = fabs(a/j);
                v = inst->beginSpeed + (a*a/2.0/j) * sign(a); //speed reached at the end of the first section
            }
            /* rest of the movement uses default acceleration */
            if (v >= 0)
            {
                a = inst->DynamicValues.AccelerationPos;
            }
            else
            {
                a = inst->DynamicValues.AccelerationNeg;
            }
            v = fabs(v);
            inst->dt[2] = (v/a)-(a/j);
            if (inst->dt[2] <= 0)
            {
                a = sqrt(v*j);
            }
            if ((a!=0)&&(v!=0))
            {
                inst->dt[1] = inst->dt[0] + a/j;
                inst->dt[2] = inst->dt[1] + v/a-a/j;
                inst->dt[3] = inst->dt[2] + a/j;
            }
            else
            {
                inst->dt[1] = 0;
                inst->dt[2] = 0;
                inst->dt[3] = 0;
            }
        }
    }

    if (inst->EStop)
    {
        inst->EStop = 0;
        if ((inst->State == STATE_MOVING)||(inst->State == STATE_STOPPING)) //stop active movement
        {
            inst->State = STATE_ESTOPPING;
            inst->beginPosition = inst->Position;
            inst->elapsedTime = 0;

            if (override > 0)
                v = inst->Speed / override;
            else
                v = 0;

            if (v >= 0)
            {
                a = inst->DynamicLimits.AccelerationPos;
            }
            else
            {
                a = inst->DynamicLimits.AccelerationNeg;
            }
            /* acceleration must be of opposite sign with speed */
            a *= -sign(v);
            inst->dt[0] = -v/a;
        }
    }
    
    /***************  STATE MACHINE ***************/

    switch (inst->State)
    {
        case STATE_STANDSTILL:
            inst->Status = STATUS_OK;
            break;

        case STATE_MOVING:
            inst->Status = 1;
            inst->elapsedTime += (cycletime * override);
            inst->Phase =0;

            if (inst->elapsedTime > inst->dt[6])
            {//movement completed
                inst->Status = STATUS_OK;
                inst->Done = 1;
                inst->State = STATE_STANDSTILL;
                if (inst->endLimits)
                {
                    inst->Status = ERROR_SVG_LIMITS_REACHED;
                }
            }
            else if (inst->elapsedTime > inst->dt[5])
            {
                inst->Phase = 2;
              //  if(axes == X) LOG_I("S7 %lf ", inst->elapsedTime);
                t = motionParam[axes].T1;
                v1 = motionParam[axes].v_start + 0.5 * j * t * t;
                inst->ds = motionParam[axes].v_start * t + (j * t * t * t / 6.0);
                t = motionParam[axes].T2;
                v2 = v1 + j * motionParam[axes].T1 * t;
         //   inst->ds += (0.5 * a_start_max * t * t + (v_start + 0.5 * a_start_max * a_start_max / j) * t);
                inst->ds += v1 * t + 0.5 * j * motionParam[axes].T1 * t * t;//S2
                t = motionParam[axes].T3;
                v3 = v2 + 0.5 * j * t * t;
        //    inst->ds += (-j * t * t * t / 6.0 + 0.5 * a_start_max * t * t + t * (v_max - 0.5 * a_start_max * a_start_max / j));
                inst->ds += v2 * t + j * t * t * t / 3.0;//S3
                t = motionParam[axes].T4;
                v4 = v3;
                inst->ds += (v3 * t);//S4
                t = motionParam[axes].T5;
                v5 = v4 - 0.5 * j * t * t;
                inst->ds += (v4 * t - (j * t * t * t / 6.0));//S5
                t = motionParam[axes].T6;
                v6 = v5 - j * motionParam[axes].T5 * t;
           // inst->ds += (v_max * t - (0.5 * a_end_max * t * t + 0.5 * a_end_max * a_end_max / j * t));
                inst->ds += v5 * t- 0.5 * j * motionParam[axes].T5 * t * t;
                t = inst->elapsedTime - inst->dt[5];
           // v7 = v6 - j * T5 * t + 0.5 * j * t * t;
           // inst->ds += (v_max * t - (-j * t * t * t / 6.0 + 0.5 * a_end_max * t * t + t * (v_max - 0.5 * a_end_max * a_end_max / j)));
                inst->ds += v6 * t - 0.5 * j * motionParam[axes].T5 * t * t + j * t * t * t / 6.0;
               // if(axes == X) LOG_I("%lf %lf\r\n", v6, inst->ds);
            }
            else if (inst->elapsedTime > inst->dt[4])
            {
                inst->Phase = 2;
                t = motionParam[axes].T1;
                v1 = motionParam[axes].v_start + 0.5 * j * t * t;
                inst->ds = motionParam[axes].v_start * t + (j * t * t * t / 6.0);
                t = motionParam[axes].T2;
                v2 = v1 + j * motionParam[axes].T1 * t;
         //   inst->ds += (0.5 * a_start_max * t * t + (v_start + 0.5 * a_start_max * a_start_max / j) * t);
                inst->ds += v1 * t + 0.5 * j * motionParam[axes].T1 * t * t;//S2
                t = motionParam[axes].T3;
                v3 = v2 + 0.5 * j * t * t;
         //   inst->ds += (-j * t * t * t / 6.0 + 0.5 * a_start_max * t * t + t * (v_max - 0.5 * a_start_max * a_start_max / j));
                inst->ds += v2 * t + j * t * t * t / 3.0;//S3
                t = motionParam[axes].T4;
                v4 = v3;
                inst->ds += (v3 * t);//S4
                t = motionParam[axes].T5;
                v5 = v4 - 0.5 * j * t * t;
                inst->ds += (v4 * t - (j * t * t * t / 6.0));//S5
                t = inst->elapsedTime - inst->dt[4]; 
          //  v6 = v5 - j * T5 * t;
           // inst->ds += (v_max * t - (0.5 * a_end_max * t * t + 0.5 * a_end_max * a_end_max / j * t));
                inst->ds += v5 * t - 0.5 * j * motionParam[axes].T5 * t * t;
         //   LOG_I("%lf %lf\r\n", v5, inst->ds);
            }
            else if (inst->elapsedTime > inst->dt[3])
            {
                inst->Phase = 2;
                //   LOG_I("S5 %lf ", inst->elapsedTime);
                t = motionParam[axes].T1;
                v1 = motionParam[axes].v_start + 0.5 * j * t * t;
                inst->ds = motionParam[axes].v_start * t + (j * t * t * t / 6.0);
                t = motionParam[axes].T2;
                v2 = v1 + j * motionParam[axes].T1 * t;
                inst->ds += v1 * t + 0.5 * j * motionParam[axes].T1 * t * t;//S2
                t = motionParam[axes].T3;
                v3 = v2 + 0.5 * j * t * t;
                inst->ds += v2 * t + j * t * t * t / 3.0;//S3
                t = motionParam[axes].T4;
                v4 = v3;
                inst->ds += (v3 * t);//S4
                t = inst->elapsedTime - inst->dt[3];   
                inst->ds += (v4 * t - (j * t * t * t / 6.0));//v4 = v_max
              //  LOG_I("%lf %lf\r\n", v4, inst->ds);
            }
            else if (inst->elapsedTime > inst->dt[2])
            {
                inst->Phase = 1;
             //   LOG_I("S4 %lf ", inst->elapsedTime);
                t = motionParam[axes].T1;
                v1 = motionParam[axes].v_start + 0.5 * j * t * t;
                inst->ds = motionParam[axes].v_start * t + (j * t * t * t / 6.0);
                t = motionParam[axes].T2;
                v2 = v1 + j * motionParam[axes].T1 * t;
          //  inst->ds += (0.5 * a_start_max * t * t + (v_start + 0.5 * a_start_max * a_start_max / j) * t);
                inst->ds += v1 * t + 0.5 * j * motionParam[axes].T1 * t * t;//S2
                t = motionParam[axes].T3;
                v3 = v2 + 0.5 * j * t * t;
          //  inst->ds += (-j * t * t * t / 6.0 + 0.5 * a_start_max * t * t + t * (v_max - 0.5 * a_start_max * a_start_max / j));
                inst->ds += v2 * t + j * t * t * t / 3.0;//S3
                t = inst->elapsedTime - inst->dt[2];
          //  v4 = v3;
                inst->ds += (v3 * t);//v3 = v_max
               // LOG_I("%lf %lf\r\n", v3, inst->ds);
            }
            else if (inst->elapsedTime > inst->dt[1])
            {
                inst->Phase = 0;
              //  LOG_I("S3 %lf ", inst->elapsedTime);
                t = motionParam[axes].T1;
                v1 = motionParam[axes].v_start + 0.5 * j * t * t;
                inst->ds = motionParam[axes].v_start * t + (j * t * t * t / 6.0);//S1
                t = motionParam[axes].T2;
                v2 = v1 + j * motionParam[axes].T1 * t;
          //  inst->ds += (0.5 * a_start_max * t * t + (v_start + 0.5 * a_start_max * a_start_max / j) * t);
                inst->ds += v1 * t + 0.5 * j * motionParam[axes].T1 * t * t;//S2
                t = inst->elapsedTime - inst->dt[1];
           // v3 = v2 + j * T1 * t - 0.5 * j * t * t;
          //  inst->ds += (-j * t * t * t / 6.0 + 0.5 * a_start_max * t * t + t * (v_max - 0.5 * a_start_max * a_start_max / j));
                inst->ds += v2 * t + 0.5 * j * motionParam[axes].T1 * t * t - j * t * t * t / 6.0;
              //  LOG_I("%lf %lf\r\n", v2, inst->ds);            
            }
            else if (inst->elapsedTime > inst->dt[0])
            {
             //   if(axes == X) LOG_I("S2 %lf ", inst->elapsedTime);
                inst->Phase = 0;
                t = motionParam[axes].T1; 
                v1 = motionParam[axes].v_start + 0.5 * j * t * t;//v1
                inst->ds = motionParam[axes].v_start * t + (j * t * t * t / 6.0);//S1
                t = inst->elapsedTime - inst->dt[0]; 
           // v2 = v1 + j * T1 * t;
           // inst->ds += (0.5 * a_start_max * t * t + (v_start + 0.5 * a_start_max * a_start_max / j) * t);
                inst->ds += v1 * t + 0.5 * j * motionParam[axes].T1 * t * t;
              //  if(axes == X) LOG_I("%lf %lf %lf %lf %lf %lf\r\n",motionParam[axes].v_start, v1, inst->ds,t,motionParam[axes].T1,inst->dt[0]);            
            }
            else
            {
              //  if(axes == X)  LOG_I("S1 %lf ", inst->elapsedTime);
                inst->Phase = 0;
                t = inst->elapsedTime;
                v1 = motionParam[axes].v_start + 0.5 * j * t * t;
                inst->ds = motionParam[axes].v_start * t + (j * t * t * t / 6.0);
             //   if(axes == X)  LOG_I("%lf %lf %lf\r\n", v_start, v1, inst->ds);
            }

            inst->Position = inst->beginPosition + (inst->ds * inst->moveDirection);
#if 0
            if (inst->Done)
            {
                inst->Position = inst->endPosition;
            }
#endif
        break;
        case STATE_STOPPING:

            inst->Status = 1;
            inst->elapsedTime += (cycletime * override);

            //movement already stopped by zero override
            if (override == 0)
            {
                inst->Status = STATUS_OK;
                inst->Done = 1;
            }

            if (inst->elapsedTime > inst->dt[3])
            {//movement completed

                //used to increase precision in case dt[0] is shorter than one cycle time
                t = inst->dt[0];
                inst->ds = (fabs(inst->beginSpeed) * t + 0.5* fabs(inst->beginAcc) * t*t - j * t*t*t / 6.0);
                t = inst->dt[1]-inst->dt[0];
                inst->ds += (v*t - (j * t*t*t /6.0));
                t = inst->dt[2]-inst->dt[1];
                inst->ds += (v*t - (0.5 * a *t*t + 0.5 * a*a/j * t));
                t = inst->dt[3] - inst->dt[2];
                inst->ds += (v*t - (-j *t*t*t /6.0 + 0.5 * a* t*t + t*(v-0.5*a*a/j)));

                inst->Status = STATUS_OK;
                inst->Done = 1;
                inst->State = STATE_STANDSTILL;
            }
            else if (inst->elapsedTime > inst->dt[2])
            {
                t = inst->dt[0];
                inst->ds = (fabs(inst->beginSpeed) * t + 0.5* fabs(inst->beginAcc) * t*t - j * t*t*t / 6.0);
                t = inst->dt[1]-inst->dt[0];
                inst->ds += (v*t - (j * t*t*t /6.0));
                t = inst->dt[2]-inst->dt[1];
                inst->ds += (v*t - (0.5 * a *t*t + 0.5 * a*a/j * t));
                t = inst->elapsedTime - inst->dt[2];
                inst->ds += (v*t - (-j *t*t*t /6.0 + 0.5 * a* t*t + t*(v-0.5*a*a/j)));
            }
            else if (inst->elapsedTime > inst->dt[1])
            {
                t = inst->dt[0];
                inst->ds = (fabs(inst->beginSpeed) * t + 0.5* fabs(inst->beginAcc) * t*t - j * t*t*t / 6.0);
                t = inst->dt[1]-inst->dt[0];
                inst->ds += (v*t - (j * t*t*t /6.0));
                t = inst->elapsedTime - inst->dt[1];
                inst->ds += (v*t - (0.5 * a *t*t + 0.5 * a*a/j * t));
            }
            else if (inst->elapsedTime > inst->dt[0])
            {
                t = inst->dt[0];
                inst->ds = (fabs(inst->beginSpeed) * t + 0.5* fabs(inst->beginAcc) * t*t - j * t*t*t / 6.0);
                t = inst->elapsedTime - inst->dt[0];
                inst->ds += (v*t - (j * t*t*t /6.0));
            }
            else
            {
                t = inst->elapsedTime;
                inst->ds = (fabs(inst->beginSpeed) * t + 0.5* fabs(inst->beginAcc) * t*t - j * t*t*t / 6.0);
            }
            inst->Position = inst->beginPosition + (inst->ds * inst->moveDirection);
           
            break;


        case STATE_ESTOPPING:

            inst->Status = 1;
            inst->elapsedTime += (cycletime * override);

            //movement already stopped by zero override
            if (override == 0)
            {
                inst->Status = STATUS_OK;
                inst->Done = 1;
            }

            if (inst->elapsedTime > inst->dt[0])
            {//movement completed
                t = inst->dt[0];
                inst->ds = (0.5*a*t*t+v*t);	//used to increase precision in case dt[0] is shorter than one cycle time
                inst->Status = STATUS_OK;
                inst->Done = 1;
                inst->State = STATE_STANDSTILL;
            }
            else
            {
                t = inst->elapsedTime;
                inst->ds = (0.5*a*t*t+v*t);
            }
            inst->Position = inst->beginPosition + inst->ds;
            break;

        case STATE_ERROR:
            //return to standstill
            inst->State = STATE_STANDSTILL;
            break;
        default:    break;
    }

    /***************  update OUTPUT values ***************/

    if (cycletime != 0)
    {
        inst->Speed = (inst->Position - oldPosition) / cycletime;
        if (inst->Done == 1)
        {
            inst->Speed = 0; //zero speed at end of movement
        }
        inst->Acceleration = (inst->Speed - oldSpeed) /cycletime;
    }
    else
    {
        inst->Speed = 0;
        inst->Acceleration = 0;
    }
	//if((inst->Done == 0)&&(axes == X)) LOG_I("%lf,%lf,%lf,%lf\r\n", inst->elapsedTime,inst->Position, inst->Speed, inst->Acceleration);
    /***************  reset all commands ***************/
    inst->Start = 0;
    inst->Stop = 0;
    inst->EStop = 0;

    /*************** simplify variable notations ***************/
    inst->v = v;
    inst->a = a;
    inst->j = j;

    return 0;
}
