#include <ch.h>
#include <hal.h>
#include <chprintf.h>

int k = 0;
char f = 1;
static void extcb( EXTDriver *extp, expchannel_t channel)
{
    extp = extp;
    channel = channel;
             if (k < 5000 && f == 1)
            	{
            		k += 1000;
            	}
            else
            {
            	f = 0;
            	k -= 1000;
            	if (k <= 0)
            	{
            		f = 1;
            	}
            }

}

static const EXTConfig extcfg = {
  .channels =
  {
    [0]  = {EXT_CH_MODE_DISABLED, NULL},
    [1]  = {EXT_CH_MODE_DISABLED, NULL},
    [2]  = {EXT_CH_MODE_DISABLED, NULL},
    [3]  = {EXT_CH_MODE_DISABLED, NULL},
    [4]  = {EXT_CH_MODE_DISABLED, NULL},
    [5]  = {EXT_CH_MODE_DISABLED, NULL},
    [6]  = {EXT_CH_MODE_DISABLED, NULL},
    [7]  = {EXT_CH_MODE_DISABLED, NULL},
    [8]  = {EXT_CH_MODE_DISABLED, NULL},
    [9]  = {EXT_CH_MODE_DISABLED, NULL},
    [10] = {EXT_CH_MODE_DISABLED, NULL},
    [11] = {EXT_CH_MODE_DISABLED, NULL},
    [12] = {EXT_CH_MODE_DISABLED, NULL},
    [13] = {EXT_CH_MODE_FALLING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOC, extcb}, //PC13 = Button
    [14] = {EXT_CH_MODE_DISABLED, NULL},
    [15] = {EXT_CH_MODE_DISABLED, NULL},
  }
};

PWMConfig pwm1conf = {
    .frequency = 500000,
    .period    = 5000,
    .callback  = NULL,
    .channels  = {
                  {PWM_OUTPUT_DISABLED, NULL},
                  {PWM_OUTPUT_DISABLED, NULL},
                  {PWM_OUTPUT_ACTIVE_HIGH, NULL},
                  {PWM_OUTPUT_DISABLED, NULL}
                  },
    .cr2        = 0,
    .dier       = 0
};

void pwm_ext_set(void)
{
	 palSetLineMode( LINE_LED1, PAL_MODE_ALTERNATE(2) );
	 pwmStart(&PWMD3 , &pwm1conf );
	 extStart( &EXTD1, &extcfg );
}

static const SerialConfig sdcfg = {
    .speed  = 115200,
    .cr1    = 0,
    .cr2    = 0,
    .cr3    = 0
};

void sd_set(void)
{
    sdStart( &SD7, &sdcfg );
    palSetPadMode( GPIOE, 8, PAL_MODE_ALTERNATE(8) );    // TX
    palSetPadMode( GPIOE, 7, PAL_MODE_ALTERNATE(8) );    // RX
}

void matlab_msg(void)
{
	 msg_t msg = sdGetTimeout( &SD7, MS2ST( 10 ) );
	                if ( msg >= 0 )
	                {
	                    int8_t byte = msg;
	                    if ( byte == 1 )
	                    {
	                        uint16_t two_bytes_number = k;
	                        sdWrite( &SD7, (uint8_t *)&two_bytes_number, sizeof( two_bytes_number ) );
	                    } else if ( byte == 2 )
	                    {
	                        uint32_t array_of_four_bytes_numbers[3] = {
	                               5, 10 , 15
	                        };
	                        sdWrite( &SD7, (uint8_t *)array_of_four_bytes_numbers, sizeof( array_of_four_bytes_numbers ) );
	                    }
	                }
}

int main(void)
{
	chSysInit();
    halInit();

    sd_set();
    pwm_ext_set();

    const uint8_t array[] = {0x53, 0x54, 0x4D,0x20, 0x49, 0x53,0x20,0x48, 0x41, 0x52,0x44,'\r','\n'};

    while (1)
    {
    	//chprintf(((BaseSequentialStream *)&SD7), "Hi\n\r");
        //sdPut(&SD7, 'f');
    	//sdWrite(&SD7, array,13);
    	//sdWrite(&SD7,(uint8_t *)"Pain\n", 6);
    	pwmEnableChannel(&PWMD3, 2 , k);
    	matlab_msg();
        chThdSleepSeconds(1);
       }
  }




