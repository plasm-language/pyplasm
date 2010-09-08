#include <scheme.h>


static Scheme_Type objscheme_class_type;

static Scheme_Object *object_struct;
static Scheme_Object *object_property;
static Scheme_Object *preparer_property;
static Scheme_Object *dispatcher_property;

#define CONS(a, b) scheme_make_pair(a, b)


typedef struct Objscheme_Class 
{
  Scheme_Type type;
  const char *name;
  Scheme_Object *sup;
  Scheme_Object *initf;
  int num_methods, num_installed;
  Scheme_Object **names;
  Scheme_Object **methods;
  Scheme_Object *base_struct_type;
  Scheme_Object *struct_type;
} 
Objscheme_Class;


Scheme_Object *scheme_module_name()
{
  /* This extension doesn't define a module: */
  return scheme_false;
}



/***************************************************************************/

static Scheme_Object *init_prim_obj(int argc, Scheme_Object **argv)
{
  Objscheme_Class *c;
  Scheme_Object *obj = argv[0];

  if (!SCHEME_STRUCTP(argv[0])|| !scheme_is_struct_instance(object_struct, argv[0]))
    scheme_wrong_type("initialize-primitive-object", "primitive-object", 0, argc, argv);
  
  c = (Objscheme_Class *)scheme_struct_type_property_ref(object_property, obj);
  return _scheme_apply(c->initf, argc, argv);
}

static Scheme_Object *class_prepare_struct_type(int argc, Scheme_Object **argv)
{
  Scheme_Object *name, *base_stype, *stype, *derive_stype;
  Scheme_Object **names, **vals, *a[3], *props;
  Objscheme_Class *c;
  int flags, count;

  if (SCHEME_TYPE(argv[0]) != objscheme_class_type)
    scheme_wrong_type("primitive-class-prepare-struct-type!", "primitive-class", 0, argc, argv);

  if (SCHEME_TYPE(argv[1]) != scheme_struct_property_type)
    scheme_wrong_type("primitive-class-prepare-struct-type!", "struct-type-property", 1, argc, argv);

  scheme_check_proc_arity("primitive-class-prepare-struct-type!", 1, 3, argc, argv);
  scheme_check_proc_arity("primitive-class-prepare-struct-type!", 2, 4, argc, argv);

  c = ((Objscheme_Class *)argv[0]);
  
  stype = c->struct_type;

  name = scheme_intern_symbol(c->name);

  if (stype) 
  {
    scheme_arg_mismatch("primitive-class-prepare-struct-type!","struct-type already prepared for primitive-class: ",name);
    return NULL;
  }

  if (SCHEME_TRUEP(c->sup) && !((Objscheme_Class *)c->sup)->base_struct_type) 
  {
    scheme_arg_mismatch("primitive-class-prepare-struct-type!","super struct-type not yet prepared for primitive-class: ",name);
    return NULL;
  }

  /* Root for this class.  */


  base_stype = scheme_make_struct_type(name, 
				       (SCHEME_TRUEP(c->sup) 
					? ((Objscheme_Class *)c->sup)->base_struct_type 
					: object_struct),
				       NULL,
				       0, 0, NULL,
				       NULL
#if MZSCHEME_VERSION_MAJOR>208
						 , NULL //guard
#endif
						 );
  c->base_struct_type = base_stype;

  /* Type to use when instantiating from C: */

  props = CONS(CONS(object_property, 
		    argv[0]),
	       scheme_null);

  stype = scheme_make_struct_type(name,
				  base_stype, 
				  NULL,
				  0, 0, NULL,
				  CONS(CONS(argv[1], argv[2]),props)
#if MZSCHEME_VERSION_MAJOR>208
				  , NULL //guard
#endif
				  );
  
  c->struct_type = stype;
  
  /* Type to derive from Scheme: */
  
  props = CONS(CONS(preparer_property, argv[3]),
	       CONS(CONS(dispatcher_property, argv[4]),
		    props));

  derive_stype = scheme_make_struct_type(name,
					 base_stype, 
					 NULL,
					 0, 0, NULL,
					 props
#if MZSCHEME_VERSION_MAJOR>208
				    , NULL //guard
#endif
						);
  
  /* Type to instantiate from Scheme: */
  
  stype = scheme_make_struct_type(name,
				  base_stype, 
				  NULL,
				  0, 0, NULL,
				  CONS(CONS(argv[1], argv[2]), props)
#if MZSCHEME_VERSION_MAJOR>208
				  , NULL //guard
#endif
				  );
  
  /* Need constructor from instantiate type: */
  flags = (SCHEME_STRUCT_NO_TYPE
	   | SCHEME_STRUCT_NO_PRED
	   | SCHEME_STRUCT_NO_GET
	   | SCHEME_STRUCT_NO_SET);
  names = scheme_make_struct_names(name, NULL, flags, &count);
  vals = scheme_make_struct_values(stype, names, count, flags);
  a[0] = vals[0];

  /* Need predicate from base type: */
  flags = (SCHEME_STRUCT_NO_TYPE
	   | SCHEME_STRUCT_NO_CONSTR
	   | SCHEME_STRUCT_NO_GET
	   | SCHEME_STRUCT_NO_SET);
  names = scheme_make_struct_names(name, NULL, flags, &count);
  vals = scheme_make_struct_values(base_stype, names, count, flags);
  a[1] = vals[0];

  /* Need derive type: */
  a[2] = derive_stype;

  return scheme_values(3, a);
}

static Scheme_Object *class_find_meth(int argc, Scheme_Object **argv)
{
  Objscheme_Class *sclass = (Objscheme_Class *)argv[0];
  Scheme_Object *s;
  int i;

  if (SCHEME_TYPE(argv[0]) != objscheme_class_type)
    scheme_wrong_type("primitive-class-find-method", "primitive-class", 0, argc, argv);
  if (!SCHEME_SYMBOLP(argv[1]))
    scheme_wrong_type("primitive-class-find-method", "symbol", 1, argc, argv);

  s = argv[1];

  for (i = sclass->num_installed; i--; ) {
    if (SAME_OBJ(sclass->names[i], s))
      return sclass->methods[i];
  }

  return scheme_false;
}

Scheme_Object *objscheme_make_uninited_object(Scheme_Object *sclass)
{
  Scheme_Object *obj;
  Scheme_Object *stype;

  stype = ((Objscheme_Class *)sclass)->struct_type;
  if (!stype) 
  {
    scheme_arg_mismatch("make-primitive-object","struct-type not yet prepared: ",sclass);
    return NULL;
  }

  obj = scheme_make_struct_instance(stype, 0, NULL);

  return obj;  
}


/***************************************************************************/
/* C-side implementation: */

Scheme_Object *objscheme_make_class(const char *name, Scheme_Object *sup, Scheme_Prim *initf, int num_methods)
{
  Objscheme_Class *sclass;
  Scheme_Object *f, **methods, **names;

  sclass = (Objscheme_Class *)scheme_malloc_tagged(sizeof(Objscheme_Class));
  sclass->type = objscheme_class_type;

  if (!sup)
    sup = scheme_false;

  sclass->name = name;
  sclass->sup = sup;

  f = scheme_make_prim(initf);
  sclass->initf = f;

  sclass->num_methods = num_methods;
  sclass->num_installed = 0;

  methods = (Scheme_Object **)scheme_malloc(sizeof(Scheme_Object *) * num_methods);
  names = (Scheme_Object **)scheme_malloc(sizeof(Scheme_Object *) * num_methods);

  sclass->methods = methods;
  sclass->names = names;

  return (Scheme_Object *)sclass;
}


Scheme_Object *objscheme_add_method_w_arity(Scheme_Object *c, const char *name,
					    Scheme_Prim *f, int mina, int maxa)
{
  Scheme_Object *s;
  Objscheme_Class *sclass;

  sclass = (Objscheme_Class *)c;
  s = scheme_make_prim_w_arity(f, name, mina + 1, (maxa < 0) ? -1 : (maxa + 1));
  sclass->methods[sclass->num_installed] = s;
  s = scheme_intern_symbol(name);
  sclass->names[sclass->num_installed] = s;
  sclass->num_installed++;
  return s;
}


//--------------------------------------------------
//giorgio scorzelli!
//--------------------------------------------------
Scheme_Object *objscheme_add_closed_method_w_arity(Scheme_Object *c, void* data,const char *name,
												   Scheme_Closed_Prim *f, int mina, int maxa)
{
  Scheme_Object *s;
  Objscheme_Class *sclass;

  sclass = (Objscheme_Class *)c;
  s = scheme_make_closed_prim_w_arity(f, data,name, mina + 1, (maxa < 0) ? -1 : (maxa + 1));
  sclass->methods[sclass->num_installed] = s;
  s = scheme_intern_symbol(name);
  sclass->names[sclass->num_installed] = s;
  sclass->num_installed++;
  return s;
}



int objscheme_is_a(Scheme_Object *o, Scheme_Object *c)
{
  Scheme_Object *a;

  if (!SCHEME_STRUCTP(o) || !scheme_is_struct_instance(object_struct, o))
    return 0;

  a = scheme_struct_type_property_ref(object_property, o);
  
  while (a && (a != c)) {
    a = ((Objscheme_Class *)a)->sup;
  }

  return !!a;
}

void objscheme_init()
{
  objscheme_class_type = scheme_make_type("<primitive-class>");

  /* Attaches a primitive class to an object: */
  scheme_register_extension_global(&object_property, sizeof(object_property));
  object_property = scheme_make_struct_type_property(scheme_intern_symbol("primitive-object"));
  
  /* Attaches a preparer function to a derived class: */
  scheme_register_extension_global(&preparer_property, sizeof(preparer_property));
  preparer_property = scheme_make_struct_type_property(scheme_intern_symbol("primitive-preparer"));

  /* Attaches a dispatcher function to a derived class: */
  scheme_register_extension_global(&dispatcher_property, sizeof(dispatcher_property));
  dispatcher_property = scheme_make_struct_type_property(scheme_intern_symbol("primitive-dispatcher"));

  /* The base struct type for the Scheme view of a primitive object: */
  scheme_register_extension_global(&object_struct, sizeof(object_struct));
  object_struct = scheme_make_struct_type(scheme_intern_symbol("primitive-object"), 
					  NULL, NULL,
					  0, 2, NULL,
					  NULL
#if MZSCHEME_VERSION_MAJOR>208
				  , NULL //guard
#endif
					  );
}

void objscheme_add_procedures(Scheme_Env *env)
{
  scheme_add_global("initialize-primitive-object",
		    scheme_make_prim_w_arity(init_prim_obj,
					     "initialize-primitive-object",
					     1, -1),
		    env);

  scheme_add_global("primitive-class-prepare-struct-type!",
		    scheme_make_prim_w_arity(class_prepare_struct_type,
					     "primitive-class-prepare-struct-type!",
					     5, 5),
		    env);
  
  scheme_add_global("primitive-class-find-method",
		    scheme_make_prim_w_arity(class_find_meth,
					     "primitive-class-find-method",
					     2, 2),
		    env);
}

Scheme_Object *objscheme_find_method(Scheme_Object *obj, char *name, void **cache)
{
  Scheme_Object *s, *p[2], *dispatcher;

  if (!obj)
    return NULL;

  dispatcher = scheme_struct_type_property_ref(dispatcher_property, (Scheme_Object *)obj);
  if (!dispatcher)
    return NULL;

  if (*cache)
    s = (Scheme_Object *)*cache;
  else {
    s = scheme_intern_symbol(name);
    p[0] = s;
    s = scheme_struct_type_property_ref(preparer_property, (Scheme_Object *)obj);
    if (!s)
      return NULL;
    s = scheme_apply(s, 1, p);
    scheme_register_extension_global((void *)cache, sizeof(Scheme_Object*));
    *cache = s;
  }

  p[0] = obj;
  p[1] = s;
  return _scheme_apply(dispatcher, 2, p);
}
