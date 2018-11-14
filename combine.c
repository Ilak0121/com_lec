
void combine1(vec_ptr v, int *dest)
{
    int i;
    *dest = 0;
    for (i = 0; i < vec_length(v); i++) {
        int val;
        get_vec_element(v, i, &val);
        *dest += val;
    }
}


void combine2 (vec_ptr v, in *dest)
{
    int i;
    int length = vec_length(v);
    *dest = 0;
    for (i =0; i < length ; i++) {
        int val;
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
     int *data = get_vec_start (v);

     int sum = 0;
     for (i = 0; i<length ; i++) {
         sum +=data [i];
     }
     *dest  +=sum;
}
