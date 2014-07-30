/*	========================================================================
	A global variable utility, given a type T, class The<T> guarantee that a
	unique global instance was accessed any time, anywhere.
		
	Usage:
		struct Thing {
			void do_something();
			int a_property;
		};
		
		The<Thing>()->do_something();
		The<Thing>()->a_property = 0;

		The<Thing>() access to the unique global instance.		
		*The<Thing>() can also get a reference.
	
	Beyong Singleton:
		The<Thing, 1>(), The<Thing, 2> can access to different instances.
		The<Thing>() is equivalent to The<Thing, 0>().
	
	To use class The, the class must have default constructor.

	========================================================================

	As a global variable, usually the memory will not be released. But you
	can delete it manually:

		delete The<Thing>();
		(Thing*)(The<Thing>()) = nullptr;
	
	Then you must not use The<Thing> again.
	
	However, deletion is not recommended, since global variables should 
	logically live in the entire execution. If you really need to do that, a
	release function is preferred:

		struct Thing {
			void release();
		};
	
		The<Thing>()->realse();
	
	For classes derived from stl containers, you already have 'swap'.
*/

#ifndef HIS_MISCELLANEOUS_THE_HPP
#define HIS_MISCELLANEOUS_THE_HPP

namespace his
{
	
template<class Cls, int = 0>
class The
{
public:
	Cls* operator->()	{ return get_instance(); }

	operator Cls*&()	{ return get_instance(); }
	
private:
	Cls *&get_instance()
	{
		static Cls *s_instance = new Cls;
		return s_instance;
	}
};


}
#endif // HIS_MISCELLANEOUS_THE_HPP
