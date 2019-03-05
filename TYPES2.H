/** PARSE.C **/
void make_pointR PROTO (( point *p, double x, double y)) ;
void do_sumR PROTO (( YYStype *ret, YYStype *a, YYStype *b )) ;
void do_subR PROTO (( YYStype *ret, YYStype *a, YYStype *b )) ;
void do_mulR PROTO (( YYStype *ret, YYStype *a, YYStype *b )) ;
void do_divR PROTO (( YYStype *ret, YYStype *a, YYStype *b )) ;
void do_negateR PROTO (( YYStype *ret, YYStype *a )) ;
void do_angleR PROTO (( YYStype *ret, YYStype *v )) ;
void do_versorR PROTO (( YYStype *ret, YYStype *v )) ;
void do_lengthR PROTO (( YYStype *ret, YYStype *v )) ;
void do_vectorR PROTO (( YYStype *ret, YYStype *a, YYStype *b )) ;
void do_getvalR PROTO (( YYStype *ret, YYStype *a )) ;
void do_gethandleR PROTO (( YYStype *ret, YYStype *a, YYStype *b )) ;
void do_assignR PROTO (( char *name, YYStype *a )) ;

/** OGG.C **/
void new_objR PROTO (( YYStype *obj_id )) ;
void check_objR PROTO (( YYStype *obj_id )) ;
void new_handleR PROTO (( YYStype *attr, YYStype *han_id )) ;
void new_down2R PROTO (( YYStype *reference, YYStype *callee )) ;
void new_down1R PROTO (( YYStype *callee )) ;
void new_arg_t1R PROTO (( YYStype *ee, YYStype *er )) ;
void new_arg_t1_defR PROTO (( YYStype *ee )) ;
void new_arg_t2R PROTO (( YYStype *ee_from, YYStype *ee_to, YYStype *er )) ;
void new_arg_t34R PROTO (( YYStype *value, int type )) ;
void set_labelR PROTO (( YYStype *label )) ;
void set_dashR PROTO (( YYStype *type )) ;
void set_fillR PROTO (( YYStype *type )) ;
void get_defvalR PROTO (( point *p, smt_node *ref )) ;
void get_valR PROTO (( point *p, smt_node *ref, char *handle_name )) ;
