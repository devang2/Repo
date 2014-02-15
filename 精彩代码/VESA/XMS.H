#if !defined XMS_H
	#define XMS_H

class XMS {
	int handle;
	int move(struct EMB *emb);
public:
	XMS(int size);
	~XMS();
	static int OK;
	static int init(void);
	static unsigned freesize(void);
	static unsigned largestblock(void);
	int realloc(int size);
	int put(void *dp,void *sp,long leng);
	int get(void *dp,void *sp,long leng);
	};

#endif

