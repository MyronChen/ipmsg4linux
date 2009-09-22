#ifndef _CONST_
#define _CONST_

/**
* the const and the macro defing
*/

/* =======================  Const Define ==================== */
//the size of message recevied.
#define MSG_SIZE 2048

//the default listening port.
#define DEF_PORT 2425

//the file numer
#define FILE_NUM 5

/* ======================== macro defing ======================= */
#define ERROR(msg) printf("%s %s:%d(%s) --ERROR:%s\n",__TIME__,__FILE__,__LINE__,__FUNCTION__,msg)

#define INFO(msg) printf("%s %s:%d(%s) ---INFO:%s\n",__TIME__,__FILE__,__LINE__,__FUNCTION__,msg)

#define ISSAPCE(x) ((x)==' '||(x)=='\r'||(x)=='\n'||(x)=='\f'||(x)=='\b'||(x)=='\t')

#define FREE(p) \
	if(p){ \
		free(p);\
		p = NULL;\
	}

#endif
