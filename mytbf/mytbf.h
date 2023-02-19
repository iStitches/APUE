#ifndef MYTBF_H__
#define MYTBF_H__

#define MYTBF_MAX 1024    //桶容量1024
typedef void mytbf_t;

mytbf_t *mytbf_init(int caps, int brust);

int mytbf_fetchtoken(mytbf_t*, int);

int mytbf_returntoken(mytbf_t*, int);

void mytbf_destory(mytbf_t*);

#endif
