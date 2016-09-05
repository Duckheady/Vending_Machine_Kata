#pragma once
/*Quick singleton defines as well, why not...*/
#define DeclareSingleton(type)															   \
	static type* instance;																   \
	static type* GetInstance(){if(instance == nullptr) instance = new type(); return instance;} \
    static void Teardown() {delete instance; instance = nullptr;}

#define DefineSingleton(type) \
	type* type::instance = nullptr;
/***/
/*Merges tokens. Use TokenMerger not TokenMerger2... I don't remember the semantics of why C++ requires
  such stupidity, but that is why I have references around*/
#ifndef TokenMerger2
#define TokenMerger2(t1, t2) t1 ## t2
#endif
#ifndef TokenMerger
#define TokenMerger(t1, t2) TokenMerger2(t1 , t2)
#endif
/***/

typedef unsigned TYPEID;
//static TYPEID type_id_counter;
TYPEID GetNextUniqueId();
#define TYPE_ID(type) getType<type>()
template<typename T>
TYPEID getType() { static TYPEID id = GetNextUniqueId(); return id; }
