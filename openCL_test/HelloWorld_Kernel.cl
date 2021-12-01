
__kernel void helloworld(__global int*pIn, __global int*pOut)

{

	int iNum = get_global_id(0);
	printf("%s\n", "this is a test string\n");
	int work_dim_all = get_local_size(0);
	int local_id = get_local_id(0);
	printf("%d\n",work_dim_all);
	printf("%d\n",local_id);
//	pOut[iNum] = pIn[iNum] + 1;
	pOut[iNum] = pIn[iNum] + pIn[iNum];
	
	
}