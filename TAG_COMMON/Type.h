#ifndef _TYPE_H_
#define _TYPE_H_

#ifdef _SIM_WIN32
	typedef unsigned char  U8;                
	typedef signed   char  S8;                     
	typedef unsigned short U16;                  
	typedef signed   short S16;                   /* defined for signed 16-bits integer variable 		�з���16λ���ͱ��� */
	typedef unsigned int   U32;                  /* defined for unsigned 32-bits integer variable 	�޷���32λ���ͱ��� */
	typedef signed   int   S32;                   /* defined for signed 32-bits integer variable 		�з���32λ���ͱ��� */

  typedef volatile  unsigned char  VU8;
	typedef  volatile signed   char  VS8;
	typedef volatile unsigned short VU16;
	typedef volatile  signed   short VS16;                   /* defined for signed 16-bits integer variable 		�з���16λ���ͱ��� */
	typedef volatile  unsigned int   VU32;                  /* defined for unsigned 32-bits integer variable 	�޷���32λ���ͱ��� */
	typedef volatile  signed   int   VS32;                   /* defined for signed 32-bits integer variable 		�з���32λ���ͱ��� */
#else
	typedef unsigned char  U8;                
	typedef signed   char  S8;                     
	typedef unsigned int U16;                  
	typedef signed   int S16;                   /* defined for signed 16-bits integer variable 		�з���16λ���ͱ��� */
	typedef unsigned long   U32;                  /* defined for unsigned 32-bits integer variable 	�޷���32λ���ͱ��� */
	typedef signed   long   S32;                   /* defined for signed 32-bits integer variable 		�з���32λ���ͱ��� */

   typedef volatile  unsigned char  VU8;
	typedef  volatile signed   char  VS8;
	typedef volatile unsigned int VU16;
	typedef volatile  signed   int VS16;                   /* defined for signed 16-bits integer variable 		�з���16λ���ͱ��� */
	typedef volatile  unsigned long   VU32;                  /* defined for unsigned 32-bits integer variable 	�޷���32λ���ͱ��� */
	typedef volatile  signed   long   VS32;                   /* defined for signed 32-bits integer variable 		�з���32λ���ͱ��� */
#endif

#define BITs(X) (1<<(X))
#define S16H(X) (U8)((X)>>8)
#define S16L(X) (U8)(X)
#ifndef NULL
    #define NULL 0
#endif
#endif
