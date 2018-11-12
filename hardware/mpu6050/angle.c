/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "stdio.h"
#include "stm32f10x.h"
#include "delay.h"
#include "sys.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "inv_mpu.h"
#include "math.h"
#include "STM32_I2C.h"
#include "angle.h"
/** @addtogroup STM32F10x_StdPeriph_Examples
  * @{
  */
/* Data requested by client. */
#define PRINT_ACCEL     (0x01)
#define PRINT_GYRO      (0x02)
#define PRINT_QUAT      (0x04)

#define ACCEL_ON        (0x01)
#define GYRO_ON         (0x02)

#define MOTION          (0)
#define NO_MOTION       (1)

/* Starting sampling rate. */
#define DEFAULT_MPU_HZ  (100)

#define FLASH_SIZE      (512)
#define FLASH_MEM_START ((void*)0x1800)

/** @addtogroup GPIO_IOToggle
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


/* Private function prototypes -----------------------------------------------*/
void Delay(__IO uint32_t nCount);
void GPIO_Configuration(void);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */

struct rx_s {
    unsigned char header[3];
    unsigned char cmd;
};
struct hal_s {
    unsigned char sensors;
    unsigned char dmp_on;
    unsigned char wait_for_tap;
    volatile unsigned char new_gyro;
    unsigned short report;
    unsigned short dmp_features;
    unsigned char motion_int_mode;
    struct rx_s rx;
};
static struct hal_s hal = {0};

/* USB RX binary semaphore. Actually, it's just a flag. Not included in struct
 * because it's declared extern elsewhere.
 */
volatile unsigned char rx_new;

/* The sensors can be mounted onto the board in any orientation. The mounting
 * matrix seen below tells the MPL how to rotate the raw data from thei
 * driver(s).
 * TODO: The following matrices refer to the configuration on an internal test
 * board at Invensense. If needed, please modify the matrices to match the
 * chip-to-body matrix for your particular set up.
 */
static signed char gyro_orientation[9] = {-1, 0, 0,
                                           0,-1, 0,
                                           0, 0, 1};

enum packet_type_e {
    PACKET_TYPE_ACCEL,
    PACKET_TYPE_GYRO,
    PACKET_TYPE_QUAT,
    PACKET_TYPE_TAP,
    PACKET_TYPE_ANDROID_ORIENT,
    PACKET_TYPE_PEDO,
    PACKET_TYPE_MISC
};
/* These next two functions converts the orientation matrix (see
 * gyro_orientation) to a scalar representation for use by the DMP.
 * NOTE: These functions are borrowed from Invensense's MPL.
 */
static  unsigned short inv_row_2_scale(const signed char *row)
{
    unsigned short b;

    if (row[0] > 0)
        b = 0;
    else if (row[0] < 0)
        b = 4;
    else if (row[1] > 0)
        b = 1;
    else if (row[1] < 0)
        b = 5;
    else if (row[2] > 0)
        b = 2;
    else if (row[2] < 0)
        b = 6;
    else
        b = 7;      // error
    return b;
}

static  unsigned short inv_orientation_matrix_to_scalar(
    const signed char *mtx)
{
    unsigned short scalar;

    /*
       XYZ  010_001_000 Identity Matrix
       XZY  001_010_000
       YXZ  010_000_001
       YZX  000_010_001
       ZXY  001_000_010
       ZYX  000_001_010
     */

    scalar = inv_row_2_scale(mtx);
    scalar |= inv_row_2_scale(mtx + 3) << 3;
    scalar |= inv_row_2_scale(mtx + 6) << 6;


    return scalar;
}


static void run_self_test(void)
{
    int result;
//    char test_packet[4] = {0};
    long gyro[3], accel[3];

    result = mpu_run_self_test(gyro, accel);
    if (result == 0x7) {
        /* Test passed. We can trust the gyro data here, so let's push it down
         * to the DMP.
         */
        float sens;
        unsigned short accel_sens;
        mpu_get_gyro_sens(&sens);
        gyro[0] = (long)(gyro[0] * sens);
        gyro[1] = (long)(gyro[1] * sens);
        gyro[2] = (long)(gyro[2] * sens);
        dmp_set_gyro_bias(gyro);
        mpu_get_accel_sens(&accel_sens);
        accel[0] *= accel_sens;
        accel[1] *= accel_sens;
        accel[2] *= accel_sens;
        dmp_set_accel_bias(accel);
		printf("setting bias succesfully ......\n");
    }
	else
	{
		printf("bias has not been modified ......\n");
	}

    /* Report results. */
//    test_packet[0] = 't';
//    test_packet[1] = result;
//    send_packet(PACKET_TYPE_MISC, test_packet);
}



/* Every time new gyro data is available, this function is called in an
 * ISR context. In this example, it sets a flag protecting the FIFO read
 * function.
 */
//static void gyro_data_ready_cb(void)
//{
//    hal.new_gyro = 1;
//}
#define q30  1073741824.0f
float q0=1.0f,q1=0.0f,q2=0.0f,q3=0.0f;

int initMPU(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f10x_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f10x.c file
     */     
  int result;     
  /* Configure all unused GPIO port pins in Analog Input mode (floating input
     trigger OFF), this will reduce the power consumption and increase the device
     immunity against EMI/EMC *************************************************/
  i2cInit();
  result = mpu_init();
  if(!result)
  {
	  //mpu_init();
	  printf("mpu initialization complete......\n ");
	  //mpu_set_sensor
	  if(!mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL))
	  {
	  	 printf("mpu_set_sensor complete ......\n");
	  }
	  else
	  {
	  	 printf("mpu_set_sensor come across error ......\n");
	  }
	  //mpu_configure_fifo
	  if(!mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL))
	  {
	  	 printf("mpu_configure_fifo complete ......\n");
	  }
	  else
	  {
	  	 printf("mpu_configure_fifo come across error ......\n");
	  }
	  //mpu_set_sample_rate
	  if(!mpu_set_sample_rate(DEFAULT_MPU_HZ))
	  {
	  	 printf("mpu_set_sample_rate complete ......\n");
	  }
	  else
	  {
	  	 printf("mpu_set_sample_rate error ......\n");
	  }
	  //dmp_load_motion_driver_firmvare
	  if(!dmp_load_motion_driver_firmware())
	  {
	  	printf("dmp_load_motion_driver_firmware complete ......\n");
	  }
	  else
	  {
	  	printf("dmp_load_motion_driver_firmware come across error ......\n");
	  }
	  //dmp_set_orientation
	  if(!dmp_set_orientation(inv_orientation_matrix_to_scalar(gyro_orientation)))
	  {
	  	 printf("dmp_set_orientation complete ......\n");
	  }
	  else
	  {
	  	 printf("dmp_set_orientation come across error ......\n");
	  }
	  //dmp_enable_feature
	  if(!dmp_enable_feature(DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_TAP |
	        DMP_FEATURE_ANDROID_ORIENT | DMP_FEATURE_SEND_RAW_ACCEL | DMP_FEATURE_SEND_CAL_GYRO |
	        DMP_FEATURE_GYRO_CAL))
	  {
	  	 printf("dmp_enable_feature complete ......\n");
	  }
	  else
	  {
	  	 printf("dmp_enable_feature come across error ......\n");
	  }
	  //dmp_set_fifo_rate
	  if(!dmp_set_fifo_rate(DEFAULT_MPU_HZ))
	  {
	  	 printf("dmp_set_fifo_rate complete ......\n");
	  }
	  else
	  {
	  	 printf("dmp_set_fifo_rate come across error ......\n");
	  }
	  run_self_test();
	  if(!mpu_set_dmp_state(1))
	  {
	  	 printf("mpu_set_dmp_state complete ......\n");
	  }
	  else
	  {
	  	 printf("mpu_set_dmp_state come across error ......\n");
	  }
	  return 0;
  }
  else{
  	return -1;
  }
}

int readAngleData(float *outPitch, float *outRoll, float *outYaw, short accelData[3]){
	unsigned long sensor_timestamp;
     short gyro[3], accel[3], sensors;
     unsigned char more;
     long quat[4];
	 float Yaw,Roll,Pitch;
	int flag = -1;
	 dmp_read_fifo(gyro, accel, quat, &sensor_timestamp, &sensors,
                &more);	 
	 /* Gyro and accel data are written to the FIFO by the DMP in chip
     * frame and hardware units. This behavior is convenient because it
     * keeps the gyro and accel outputs of dmp_read_fifo and
     * mpu_read_fifo consistent.
     */
/*     if (sensors & INV_XYZ_GYRO )
       send_packet(PACKET_TYPE_GYRO, gyro);
     if (sensors & INV_XYZ_ACCEL)
        send_packet(PACKET_TYPE_ACCEL, accel); */
     /* Unlike gyro and accel, quaternions are written to the FIFO in
     * the body frame, q30. The orientation is set by the scalar passed
     * to dmp_set_orientation during initialization.
     */
     if (sensors & INV_WXYZ_QUAT )
	 {
	     printf("in Calculating quaternion steps.....\n");
	 	 q0=quat[0] / q30;
		 q1=quat[1] / q30;
		 q2=quat[2] / q30;
		 q3=quat[3] / q30;
		 Pitch  = asin(-2 * q1 * q3 + 2 * q0* q2)* 57.3; // pitch
		 Roll = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2* q2 + 1)* 57.3; // roll
		 Yaw = 	atan2(2*(q1*q2 + q0*q3),q0*q0+q1*q1-q2*q2-q3*q3) * 57.3;
		 
		 *outPitch 		= Pitch;
		 *outRoll		= Roll;
		 *outYaw		= Yaw; 
		 flag = 1;
	 } else {
		 flag = -1;
	 }
	 if(sensors & INV_XYZ_GYRO)
	 {}
	 if(sensors & INV_XYZ_ACCEL)
	 {
		for(int i=0; i<3; i++){
			accelData[i] = accel[i];		// 读取加速度数据
		}
	 }
      //send_packet(PACKET_TYPE_QUAT, quat);
	 return flag;
}
/**
  * @brief  Inserts a delay time.
  * @param  nCount: specifies the delay time length.
  * @retval None
  */
void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
