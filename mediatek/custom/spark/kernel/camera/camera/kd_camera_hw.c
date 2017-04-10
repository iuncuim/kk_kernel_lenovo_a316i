#include <linux/videodev2.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <asm/atomic.h>
#include <linux/xlog.h>

#include "kd_camera_hw.h"

#include "kd_imgsensor.h"
#include "kd_imgsensor_define.h"
#include "kd_camera_feature.h"

/******************************************************************************
 * Debug configuration
******************************************************************************/
#define PFX "[kd_camera_hw]"
#define PK_DBG_NONE(fmt, arg...)    do {} while (0)
//#define PK_DBG_FUNC(fmt, arg...)    printk(KERN_INFO PFX "%s: " fmt, __FUNCTION__ ,##arg)
#define PK_DBG_FUNC printk

#define DEBUG_CAMERA_HW_K
#ifdef  DEBUG_CAMERA_HW_K
#define PK_DBG PK_DBG_FUNC
#define PK_ERR(fmt, arg...)         printk(KERN_ERR PFX "%s: " fmt, __FUNCTION__ ,##arg)
#define PK_XLOG_INFO(fmt, args...) \
        do {    \
                    xlog_printk(ANDROID_LOG_INFO, "kd_camera_hw", fmt, ##args); \
        } while(0)
#else
#define PK_DBG(a,...)
#define PK_ERR(a,...)
#define PK_XLOG_INFO(fmt, args...)
#endif


/*#ifndef GPIO_MAIN_CAMERA_28V_POWER_CTRL_PIN 
#define GPIO_MAIN_CAMERA_28V_POWER_CTRL_PIN GPIO37
#endif 
static void MainCameraAnalogPowerCtrl(kal_bool on){
    if(mt_set_gpio_mode(GPIO_MAIN_CAMERA_28V_POWER_CTRL_PIN,0)){PK_DBG("[[CAMERA SENSOR] Set MAIN CAMERA_DIGITAL POWER_PIN ! \n");}
    if(mt_set_gpio_dir(GPIO_MAIN_CAMERA_28V_POWER_CTRL_PIN,GPIO_DIR_OUT)){PK_DBG("[[CAMERA SENSOR] Set CAMERA_POWER_PULL_PIN DISABLE ! \n");}
    if(mt_set_gpio_out(GPIO_MAIN_CAMERA_28V_POWER_CTRL_PIN,on)){PK_DBG("[[CAMERA SENSOR] Set CAMERA_POWER_PULL_PIN DISABLE ! \n");;}
}*/



int kdCISModulePowerOn(CAMERA_DUAL_CAMERA_SENSOR_ENUM SensorIdx, char *currSensorName, BOOL On, char* mode_name)
{
    u32 pinSetIdx = 0;//default main sensor
    u32 pinSetIdxTmp = 0;

    #define IDX_PS_CMRST 0
    #define IDX_PS_CMPDN 4

    #define IDX_PS_MODE 1
    #define IDX_PS_ON   2
    #define IDX_PS_OFF  3
    u32 pinSet[2][8] = {
          //for main sensor
          {GPIO_CAMERA_CMRST_PIN,
              GPIO_CAMERA_CMRST_PIN_M_GPIO,   /* mode */
              GPIO_OUT_ONE,                   /* ON state */
              GPIO_OUT_ZERO,                  /* OFF state */
           GPIO_CAMERA_CMPDN_PIN,
              GPIO_CAMERA_CMPDN_PIN_M_GPIO,
              GPIO_OUT_ZERO,	//GPIO_OUT_ONE,
              GPIO_OUT_ONE,	//GPIO_OUT_ZERO,
          },
          //for sub sensor
          {GPIO_CAMERA_CMRST1_PIN,
           GPIO_CAMERA_CMRST1_PIN_M_GPIO,
              GPIO_OUT_ONE,
              GPIO_OUT_ZERO,
           GPIO_CAMERA_CMPDN1_PIN,
              GPIO_CAMERA_CMPDN1_PIN_M_GPIO,
              GPIO_OUT_ZERO,	//GPIO_OUT_ONE, 
              GPIO_OUT_ONE,	//GPIO_OUT_ZERO,
          }
         };

    if (DUAL_CAMERA_MAIN_SENSOR == SensorIdx){
        pinSetIdx = 0;
    }
    else if (DUAL_CAMERA_SUB_SENSOR == SensorIdx) {
        pinSetIdx = 1;
    }


    //power ON
    if (On) {

       PK_DBG("kdCISModulePowerOn -on:currSensorName=%s;\n",currSensorName);

	   //MainCameraDigtalPowerCtrl(1);
       
       if (currSensorName && (0 == strcmp(SENSOR_DRVNAME_OV2659_YUV,currSensorName)))
       {
		   PK_DBG("[CAMERA SENSOR] kdCISModulePowerOn get in---SENSOR_DRVNAME_OV2659_YUV sensorIdx:%d; pinSetIdx=%d\n",SensorIdx, pinSetIdx);
	   
		   if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_D2, VOL_1800,mode_name))
		   {
			   PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");//2.8 ->1.8
			   //return -EIO;
			   goto _kdCISModulePowerOn_exit_;
		   }
	   	   mdelay(10);

		   if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_A, VOL_2800,mode_name))
		   {
			   PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
			   //return -EIO;
			   goto _kdCISModulePowerOn_exit_;
		   }
	           mdelay(10);

		   if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_D, VOL_1500,mode_name))
		   {
			   PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");
			   //return -EIO;
			   goto _kdCISModulePowerOn_exit_;
		   }	   
	   	   mdelay(10);

		   if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_A2, VOL_2800,mode_name))
		   {
			   PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
			   //return -EIO;
			   goto _kdCISModulePowerOn_exit_;
		   }
		   
		   msleep(10);

        	//enable active sensor
        	if (GPIO_CAMERA_INVALID != pinSet[pinSetIdx][IDX_PS_CMRST]) {
            	    if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_MODE])){PK_DBG("[CAMERA 				SENSOR] set gpio mode failed!! \n");}
            	    if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMRST],GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed\n");}
            	    if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_OFF])){PK_DBG("[CAMERA SENSOR] set gpio 				failed!! \n");}
            	    mdelay(10);
            	    if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_ON])){PK_DBG("[CAMERA SENSOR] set gpio 				failed!! \n");}
            	    mdelay(1);
		    PK_DBG("kdCISModulePowerOn enable active sensor set GPIO_CAMERA_CMRST_PIN(GPIO%d) on\n",pinSet[pinSetIdx][IDX_PS_CMRST]);
            	    //PDN pin
            	    if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_MODE])){PK_DBG("[CAMERA LENS] set gpio 				mode failed!! \n");}
            	    if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMPDN],GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
            	    if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_ON])){PK_DBG("[CAMERA LENS] set gpio 				failed!! \n");}
 		    PK_DBG("kdCISModulePowerOn enable active sensor set GPIO_CAMERA_CMPDN_PIN(GPIO%d) on\n",pinSet[pinSetIdx][IDX_PS_CMPDN]);
        	}  
		   
		   //disable inactive sensor
		   if(pinSetIdx == 0) {//disable sub
			   pinSetIdxTmp = 1;
		   }
		   else{
			   pinSetIdxTmp = 0;
		   }
		   if (GPIO_CAMERA_INVALID != pinSet[pinSetIdxTmp][IDX_PS_CMRST]) {
			   if(mt_set_gpio_mode(pinSet[pinSetIdxTmp][IDX_PS_CMRST],pinSet[pinSetIdxTmp][IDX_PS_CMRST+IDX_PS_MODE])){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
			   if(mt_set_gpio_mode(pinSet[pinSetIdxTmp][IDX_PS_CMPDN],pinSet[pinSetIdxTmp][IDX_PS_CMPDN+IDX_PS_MODE])){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}
			   if(mt_set_gpio_dir(pinSet[pinSetIdxTmp][IDX_PS_CMRST],GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
			   if(mt_set_gpio_dir(pinSet[pinSetIdxTmp][IDX_PS_CMPDN],GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
			   if(mt_set_gpio_out(pinSet[pinSetIdxTmp][IDX_PS_CMRST],pinSet[pinSetIdxTmp][IDX_PS_CMRST+IDX_PS_OFF])){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");} 
                //LINE: <> <20130709> <solve the subcamera distrub the mainCamera in result in ripple> panzaoyan
              //if(mt_set_gpio_out(pinSet[pinSetIdxTmp][IDX_PS_CMPDN],pinSet[pinSetIdxTmp][IDX_PS_CMPDN+IDX_PS_ON])){PK_DBG("[CAMERA LENS] set gpio failed!! \n");} 
			   if(mt_set_gpio_out(pinSet[pinSetIdxTmp][IDX_PS_CMPDN],pinSet[pinSetIdxTmp][IDX_PS_CMPDN+IDX_PS_OFF])){PK_DBG("[CAMERA LENS] set gpio failed!! \n");} 

			PK_DBG("kdCISModulePowerOn disable inactive sensor --> set CMRST(GPIO%d) and CMPDN(GPIO%d) off\n",
				pinSet[pinSetIdxTmp][IDX_PS_CMRST],pinSet[pinSetIdxTmp][IDX_PS_CMPDN]);
		   }
	   }
	   if (currSensorName && (0 == strcmp(SENSOR_DRVNAME_HI253_YUV,currSensorName)))
       {
		   PK_DBG("[CAMERA SENSOR] kdCISModulePowerOn get in---SENSOR_DRVNAME_HI253_YUV sensorIdx:%d; pinSetIdx=%d\n",SensorIdx, pinSetIdx);
	   
		   if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_D2, VOL_1800,mode_name))
		   {
			   PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");//2.8 ->1.8
			   //return -EIO;
			   goto _kdCISModulePowerOn_exit_;
		   }
	   	   mdelay(10);

		   if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_A, VOL_2800,mode_name))
		   {
			   PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
			   //return -EIO;
			   goto _kdCISModulePowerOn_exit_;
		   }
	           mdelay(10);

		 
		  if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_D, VOL_1800,mode_name)) //LINE: <VOL_1500> <20130704> <modify the DVDD> panzaoyan
		   {
			   PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");
			   //return -EIO;
			   goto _kdCISModulePowerOn_exit_;
		   }	   
	   	   mdelay(10);

		   if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_A2, VOL_2800,mode_name))
		   {
			   PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
			   //return -EIO;
			   goto _kdCISModulePowerOn_exit_;
		   }
		   
		   msleep(10);

        	//enable active sensor
        	if (GPIO_CAMERA_INVALID != pinSet[pinSetIdx][IDX_PS_CMRST]) {
            	    if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_MODE])){PK_DBG("[CAMERA 				SENSOR] set gpio mode failed!! \n");}
            	    if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMRST],GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed\n");}
            	    if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_OFF])){PK_DBG("[CAMERA SENSOR] set gpio 				failed!! \n");}
            	    mdelay(10);
            	    if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_ON])){PK_DBG("[CAMERA SENSOR] set gpio 				failed!! \n");}
            	    mdelay(1);
		    PK_DBG("kdCISModulePowerOn enable active sensor set GPIO_CAMERA_CMRST_PIN(GPIO%d) on\n",pinSet[pinSetIdx][IDX_PS_CMRST]);
            	    //PDN pin
            	    if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_MODE])){PK_DBG("[CAMERA LENS] set gpio 				mode failed!! \n");}
            	    if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMPDN],GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
            	    if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_ON])){PK_DBG("[CAMERA LENS] set gpio 				failed!! \n");}
 		    PK_DBG("kdCISModulePowerOn enable active sensor set GPIO_CAMERA_CMPDN_PIN(GPIO%d) on\n",pinSet[pinSetIdx][IDX_PS_CMPDN]);
        	}  
		   
		   //disable inactive sensor
		   if(pinSetIdx == 0) {//disable sub
			   pinSetIdxTmp = 1;
		   }
		   else{
			   pinSetIdxTmp = 0;
		   }
		   if (GPIO_CAMERA_INVALID != pinSet[pinSetIdxTmp][IDX_PS_CMRST]) {
			   if(mt_set_gpio_mode(pinSet[pinSetIdxTmp][IDX_PS_CMRST],pinSet[pinSetIdxTmp][IDX_PS_CMRST+IDX_PS_MODE])){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
			   if(mt_set_gpio_mode(pinSet[pinSetIdxTmp][IDX_PS_CMPDN],pinSet[pinSetIdxTmp][IDX_PS_CMPDN+IDX_PS_MODE])){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}
			   if(mt_set_gpio_dir(pinSet[pinSetIdxTmp][IDX_PS_CMRST],GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
			   if(mt_set_gpio_dir(pinSet[pinSetIdxTmp][IDX_PS_CMPDN],GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
			   if(mt_set_gpio_out(pinSet[pinSetIdxTmp][IDX_PS_CMRST],pinSet[pinSetIdxTmp][IDX_PS_CMRST+IDX_PS_OFF])){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");} 
			   if(mt_set_gpio_out(pinSet[pinSetIdxTmp][IDX_PS_CMPDN],pinSet[pinSetIdxTmp][IDX_PS_CMPDN+IDX_PS_ON])){PK_DBG("[CAMERA LENS] set gpio failed!! \n");} 

			PK_DBG("kdCISModulePowerOn disable inactive sensor --> set CMRST(GPIO%d) and CMPDN(GPIO%d) off\n",
				pinSet[pinSetIdxTmp][IDX_PS_CMRST],pinSet[pinSetIdxTmp][IDX_PS_CMPDN]);
		   }
	   }	   
// LuckAs change
           else if (currSensorName && (0 == strcmp(SENSOR_DRVNAME_HI704_YUV,currSensorName)))
// orig
//           else if (currSensorName && (0 == strcmp(SENSOR_DRVNAME_SP0A19_YUV,currSensorName)))
           {  
	      PK_DBG("[CAMERA SENSOR] kdCISModulePowerOn get in---SENSOR_DRVNAME_HI704_YUV sensorIdx:%d; pinSetIdx=%d\n",SensorIdx, pinSetIdx);
// orig
//	      PK_DBG("[CAMERA SENSOR] kdCISModulePowerOn get in---SENSOR_DRVNAME_SP0A19_YUV sensorIdx:%d; pinSetIdx=%d\n",SensorIdx, pinSetIdx);
// LuckAs end
       
             if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_A, VOL_2800,mode_name))
              {
                 PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
                 //return -EIO;
                 goto _kdCISModulePowerOn_exit_;
              }
		  mdelay(10);  

		if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_A2, VOL_2800,mode_name))
             {
                 PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
                 //return -EIO;
                 goto _kdCISModulePowerOn_exit_;
              }
              mdelay(10);

              if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_D, VOL_1500,mode_name))
              {
                 PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");
                 //return -EIO;
                 goto _kdCISModulePowerOn_exit_;
              }
              mdelay(10);

	        if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_D2, VOL_1800/*VOL_2800*/,mode_name))
              {
                  PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");
                  //return -EIO;
             	goto _kdCISModulePowerOn_exit_;
              }
              mdelay(10);       

             //enable active sensor
             if (GPIO_CAMERA_INVALID != pinSet[pinSetIdx][IDX_PS_CMRST]) {
                 if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_MODE])){PK_DBG("[CAMERA 			 	SENSOR] set gpio mode failed!! \n");}
            	 if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMRST],GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed\n");}
            	 if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_OFF])){PK_DBG("[CAMERA SENSOR] set gpio 				failed!! \n");}
            	 mdelay(10);
            	 if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_ON])){PK_DBG("[CAMERA SENSOR] set gpio 				failed!! \n");}
            	 mdelay(1);
		 PK_DBG("kdCISModulePowerOn enable active sensor set GPIO_CAMERA_CMRST_PIN(GPIO%d) on\n",pinSet[pinSetIdx][IDX_PS_CMRST]);
            	 //PDN pin
            	 if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_MODE])){PK_DBG("[CAMERA LENS] set gpio 				mode failed!! \n");}
            	 if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMPDN],GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
            	 if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_ON])){PK_DBG("[CAMERA LENS] set gpio 			failed!! \n");}            	 
 		 PK_DBG("kdCISModulePowerOn enable active sensor set GPIO_CAMERA_CMPDN_PIN(GPIO%d) on\n",pinSet[pinSetIdx][IDX_PS_CMPDN]);
		 mdelay(10);
		 if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_OFF])){PK_DBG("[CAMERA LENS] set gpio 			failed!! \n");}            	 
 		 PK_DBG("kdCISModulePowerOn enable active sensor set GPIO_CAMERA_CMPDN_PIN(GPIO%d) on\n",pinSet[pinSetIdx][IDX_PS_CMPDN]);
		 mdelay(10);
		 if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_ON])){PK_DBG("[CAMERA LENS] set gpio 			failed!! \n");}            	 
 		 PK_DBG("kdCISModulePowerOn enable active sensor set GPIO_CAMERA_CMPDN_PIN(GPIO%d) on\n",pinSet[pinSetIdx][IDX_PS_CMPDN]);
		 mdelay(100);
             }  
       
             //disable inactive sensor
             if(pinSetIdx == 0) {//disable sub
                 pinSetIdxTmp = 1;
             }
             else{
                 pinSetIdxTmp = 0;
             }
             if (GPIO_CAMERA_INVALID != pinSet[pinSetIdxTmp][IDX_PS_CMRST]) {
               	if(mt_set_gpio_mode(pinSet[pinSetIdxTmp][IDX_PS_CMRST],pinSet[pinSetIdxTmp][IDX_PS_CMRST+IDX_PS_MODE])){PK_DBG("[CAMERA 		    SENSOR] set gpio mode failed!! \n");}
             	if(mt_set_gpio_mode(pinSet[pinSetIdxTmp][IDX_PS_CMPDN],pinSet[pinSetIdxTmp][IDX_PS_CMPDN+IDX_PS_MODE])){PK_DBG("[CAMERA LENS] set gpio 			    mode failed!! \n");}
             	if(mt_set_gpio_dir(pinSet[pinSetIdxTmp][IDX_PS_CMRST],GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
             	if(mt_set_gpio_dir(pinSet[pinSetIdxTmp][IDX_PS_CMPDN],GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
             	if(mt_set_gpio_out(pinSet[pinSetIdxTmp][IDX_PS_CMRST],pinSet[pinSetIdxTmp][IDX_PS_CMRST+IDX_PS_OFF])){PK_DBG("[CAMERA SENSOR] set gpio 			    failed!! \n");} 
             	if(pinSetIdx == 0){//when scan sp0a19 for main
			         if(mt_set_gpio_out(pinSet[pinSetIdxTmp][IDX_PS_CMPDN],pinSet[pinSetIdxTmp][IDX_PS_CMPDN+IDX_PS_OFF])){PK_DBG("[CAMERA LENS] set gpio 			failed!! \n");}
			     }else if(pinSetIdx == 1){////when scan sp0a19 for sub
				     if(mt_set_gpio_out(pinSet[pinSetIdxTmp][IDX_PS_CMPDN],pinSet[pinSetIdxTmp][IDX_PS_CMPDN+IDX_PS_ON])){PK_DBG("[CAMERA LENS] set gpio 			failed!! \n");}
		         } 
		PK_DBG("kdCISModulePowerOn disable inactive sensor --> set CMRST(GPIO%d) and CMPDN(GPIO%d) off\n",
				pinSet[pinSetIdxTmp][IDX_PS_CMRST],pinSet[pinSetIdxTmp][IDX_PS_CMPDN]);
             }          
       }     
       else if (currSensorName && (0 == strcmp(SENSOR_DRVNAME_OV7675_YUV,currSensorName)))
       {  
	  PK_DBG("[CAMERA SENSOR] kdCISModulePowerOn get in---SENSOR_DRVNAME_OV7675_YUV sensorIdx:%d; pinSetIdx=%d\n",SensorIdx, pinSetIdx);
       
          if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_D2, VOL_1800/*VOL_2800*/,mode_name))
          {
             PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");
             //return -EIO;
             goto _kdCISModulePowerOn_exit_;
          }
       	  mdelay(10);

          if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_A, VOL_2800,mode_name))
          {
             PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
             //return -EIO;
             goto _kdCISModulePowerOn_exit_;
          }
          mdelay(10);

          if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_D, VOL_1500,mode_name))
          {
             PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");
             //return -EIO;
             goto _kdCISModulePowerOn_exit_;
          }
          mdelay(10);

          if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_A2, VOL_2800,mode_name))
          {
             PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
             //return -EIO;
             goto _kdCISModulePowerOn_exit_;
          }
          mdelay(10);

             //enable active sensor
             if (GPIO_CAMERA_INVALID != pinSet[pinSetIdx][IDX_PS_CMRST]) {
                 if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_MODE])){PK_DBG("[CAMERA 			 	SENSOR] set gpio mode failed!! \n");}
            	 if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMRST],GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed\n");}
            	 if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_OFF])){PK_DBG("[CAMERA SENSOR] set gpio 				failed!! \n");}
            	 mdelay(10);
            	 if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_ON])){PK_DBG("[CAMERA SENSOR] set gpio 				failed!! \n");}
            	 mdelay(1);
		 PK_DBG("kdCISModulePowerOn enable active sensor set GPIO_CAMERA_CMRST_PIN(GPIO%d) on\n",pinSet[pinSetIdx][IDX_PS_CMRST]);
            	 //PDN pin
            	 if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_MODE])){PK_DBG("[CAMERA LENS] set gpio 				mode failed!! \n");}
            	 if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMPDN],GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
            	 if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_ON])){PK_DBG("[CAMERA LENS] set gpio 			failed!! \n");}
 		 PK_DBG("kdCISModulePowerOn enable active sensor set GPIO_CAMERA_CMPDN_PIN(GPIO%d) on\n",pinSet[pinSetIdx][IDX_PS_CMPDN]);
             } 
       
          //disable inactive sensor
          if(pinSetIdx == 0) {//disable sub
             pinSetIdxTmp = 1;
          }
          else{
             pinSetIdxTmp = 0;
          }
          if (GPIO_CAMERA_INVALID != pinSet[pinSetIdxTmp][IDX_PS_CMRST]) {
             if(mt_set_gpio_mode(pinSet[pinSetIdxTmp][IDX_PS_CMRST],pinSet[pinSetIdxTmp][IDX_PS_CMRST+IDX_PS_MODE])){PK_DBG("[CAMERA SENSOR] set gpio 			mode failed!! \n");}
             if(mt_set_gpio_mode(pinSet[pinSetIdxTmp][IDX_PS_CMPDN],pinSet[pinSetIdxTmp][IDX_PS_CMPDN+IDX_PS_MODE])){PK_DBG("[CAMERA LENS] set gpio 			mode failed!! \n");}
             if(mt_set_gpio_dir(pinSet[pinSetIdxTmp][IDX_PS_CMRST],GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
             if(mt_set_gpio_dir(pinSet[pinSetIdxTmp][IDX_PS_CMPDN],GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
             if(mt_set_gpio_out(pinSet[pinSetIdxTmp][IDX_PS_CMRST],pinSet[pinSetIdxTmp][IDX_PS_CMRST+IDX_PS_OFF])){PK_DBG("[CAMERA SENSOR] set gpio 			failed!! \n");} 
             if(pinSetIdx == 0){//when hi704 scan for main
			     if(mt_set_gpio_out(pinSet[pinSetIdxTmp][IDX_PS_CMPDN],pinSet[pinSetIdxTmp][IDX_PS_CMPDN+IDX_PS_OFF])){PK_DBG("[CAMERA LENS] set gpio 			failed!! \n");}
			 }else if(pinSetIdx == 1){////when hi704 scan for sub
				 if(mt_set_gpio_out(pinSet[pinSetIdxTmp][IDX_PS_CMPDN],pinSet[pinSetIdxTmp][IDX_PS_CMPDN+IDX_PS_ON])){PK_DBG("[CAMERA LENS] set gpio 			failed!! \n");}
		     }
             
	     PK_DBG("kdCISModulePowerOn disable inactive sensor --> set CMRST(GPIO%d) and CMPDN(GPIO%d) off\n",
				pinSet[pinSetIdxTmp][IDX_PS_CMRST],pinSet[pinSetIdxTmp][IDX_PS_CMPDN]);
          }          
       }
    }
    else {//power OFF

       PK_DBG("kdCISModulePowerOn -off:currSensorName=%s\n",currSensorName);
       if (GPIO_CAMERA_INVALID != pinSet[pinSetIdx][IDX_PS_CMRST]) {
           if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_MODE])){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
           if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMRST],GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
           if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_OFF])){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");} //low == reset sensor
       
           if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_MODE])){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}
           if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMPDN],GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
           if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_ON])){PK_DBG("[CAMERA LENS] set gpio failed!! \n");} //high == power down lens module
           PK_DBG("kdCISModulePowerOn -off sensor --> set CMRST(GPIO%d) and CMPDN(GPIO%d) off\n",
				pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMPDN]);
       }

       if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_D, mode_name)) {
           PK_DBG("[CAMERA SENSOR] Fail to OFF digital power\n");
           //return -EIO;
           goto _kdCISModulePowerOn_exit_;
       }
       
       if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_A,mode_name)) {
           PK_DBG("[CAMERA SENSOR] Fail to OFF analog power\n");
           //return -EIO;
           goto _kdCISModulePowerOn_exit_;
       }
       
       if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_D2,mode_name))
       {
           PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
           //return -EIO;
           goto _kdCISModulePowerOn_exit_;
       }
       
       if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_A2,mode_name))
       {
           PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
           //return -EIO;
           goto _kdCISModulePowerOn_exit_;
       }
  }

  return 0;
_kdCISModulePowerOn_exit_:
    return -EIO;
}


EXPORT_SYMBOL(kdCISModulePowerOn);

//!--
//
