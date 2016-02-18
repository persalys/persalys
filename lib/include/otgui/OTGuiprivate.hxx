
#ifndef OTGUI_PRIVATE_HXX
#define OTGUI_PRIVATE_HXX

/* From http://gcc.gnu.org/wiki/Visibility */
/* Generic helper definitions for shared library support */
#if defined _WIN32 || defined __CYGWIN__
#define OTGUI_HELPER_DLL_IMPORT __declspec(dllimport)
#define OTGUI_HELPER_DLL_EXPORT __declspec(dllexport)
#define OTGUI_HELPER_DLL_LOCAL
#else
#if __GNUC__ >= 4
#define OTGUI_HELPER_DLL_IMPORT __attribute__ ((visibility ("default")))
#define OTGUI_HELPER_DLL_EXPORT __attribute__ ((visibility ("default")))
#define OTGUI_HELPER_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
#else
#define OTGUI_HELPER_DLL_IMPORT
#define OTGUI_HELPER_DLL_EXPORT
#define OTGUI_HELPER_DLL_LOCAL
#endif
#endif

/* Now we use the generic helper definitions above to define OTGUI_API and OTGUI_LOCAL.
 * OTGUI_API is used for the public API symbols. It either DLL imports or DLL exports (or does nothing for static build)
 * OTGUI_LOCAL is used for non-api symbols. */

#ifndef OTGUI_STATIC /* defined if OT is compiled as a DLL */
#ifdef OTGUI_DLL_EXPORTS /* defined if we are building the OT DLL (instead of using it) */
#define OTGUI_API OTGUI_HELPER_DLL_EXPORT
#else
#define OTGUI_API OTGUI_HELPER_DLL_IMPORT
#endif /* OTGUI_DLL_EXPORTS */
#define OTGUI_LOCAL OTGUI_HELPER_DLL_LOCAL
#else /* OTGUI_STATIC is defined: this means OT is a static lib. */
#define OTGUI_API
#define OTGUI_LOCAL
#endif /* !OTGUI_STATIC */


#endif // OTGUI_PRIVATE_HXX

