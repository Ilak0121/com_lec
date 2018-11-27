
typedef struct {
	int len;
	long int *data;
} vec_rec, *vec_ptr;

int vec_length(vec_ptr v)
{
	return v->len;
}

long int *get_vec_start(vec_ptr v)
{
	return v->data;
}

int get_vec_element (vec_ptr v, int index, long int *dest)
{
	if (index < 0 || index >= v-> len)
		return 0;
	*dest = v->data[index];
	return 1;
}


vec_ptr new_vec(int len)
{
	vec_ptr result = (vec_ptr)malloc(sizeof(vec_rec));
	if(!result)
		return NULL;
	result->len = len;

	if (len >0) {
		long int *data = (long int*) calloc(len, sizeof(long int));

		if(!data) {
			free((void *) result);
			return NULL;
		}
		result-> data = data;
	}
	else
		result -> data = NULL;
	return result;
}

void combine1(vec_ptr v, int *dest)
{
	int i;
	*dest = 0;
	for (i = 0; i < vec_length(v); i++) {
		long int val;
		get_vec_element(v, i, &val);
	*dest += val;
	}
}

void combine2 (vec_ptr v, int *dest)
{
	int i;
	int length = vec_length(v);
	*dest = 0;
		for (i =0; i < length ; i++) {
			long int val;
			get_vec_element (v,i,&val);
			*dest += val;
		}
}

void combine3 (vec_ptr v, int *dest)
{
	int i;
	int length = vec_length(v);
	int *data = get_vec_start(v);
	*dest = 0;
	for ( i =0; i < length ; i++) {
		*dest += data[i];
	}
}


void combine4 (vec_ptr v, int *dest) {
	 int i;
	int length = vec_length(v);
	long int *data = get_vec_start (v);

	int sum = 0;
	for (i = 0; i<length ; i++) {
		sum +=data [i];
	}
	*dest  +=sum;
}
