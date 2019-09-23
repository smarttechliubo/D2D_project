/** @file typedef.h
*
* @brief  d2d protocol type definition
* @author   bo.liu
* @date     2019/07/29
* COPYRIGHT NOTICE: (c) smartlogictech.
* All rights reserved.
*/

#ifndef INC_TYPEDEF_H_
#define INC_TYPEDEF_H_

#define true 1
#define false 0

typedef signed char		int8_t;
typedef unsigned char	uint8_t;
typedef signed short	int16_t;
typedef unsigned short	uint16_t;
typedef signed int		int32_t;
typedef unsigned int	uint32_t;

typedef   int   MessagesIds; 
typedef   int   instance_t;


typedef int                bool;
typedef uint32_t           frame_t;
typedef uint32_t           sub_frame_t;
typedef uint16_t           rnti_t;




typedef enum
{/**1:tm_dl,2:tm_ul, 3:tm ul&dl, 3:um_dl,4:um_ul, 5:um_dl&ul **/
   RLC_MODE_NONE,
   RLC_MODE_TM, 
   RLC_MODE_TM_DL,      //!0
   RLC_MODE_TM_UL,
   RLC_MODE_UM, 
   RLC_MODE_UM_DL, 
   RLC_MODE_UM_UL, 
   RLC_MODE_AM
}rlc_mode_e; 


#endif /* INC_TYPEDEF_H_ */
