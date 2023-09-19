// This code has been inspired by the internals of LAGrapgh.
// The same concepts are used here, but the code has been adapted to
// the needs of DynamicClosure.

// [Reference]
// https://github.com/GraphBLAS/LAGraph/blob/102bd15a0f842b7e3d8d2f2eed5b4728fd5811c9/src/utility/LG_internal.h#L60

#include "DynamicClosure.h"

//------------------------------------------------------------------------------
// DYC_CLEAR_MSG: clear the error msg string
//------------------------------------------------------------------------------

#define DYC_CLEAR_MSG                   \
    {                                   \
        if (msg != NULL) msg[0] = '\0'; \
    }

//------------------------------------------------------------------------------
// DYC_FREE_WORK: free all workspace
//------------------------------------------------------------------------------

#ifndef DYC_FREE_WORK
#define DYC_FREE_WORK ;
#endif

//------------------------------------------------------------------------------
// DYC_FREE_ALL: free all workspace and all output arguments, on error
//------------------------------------------------------------------------------

#ifndef DYC_FREE_ALL
#define DYC_FREE_ALL \
    { DYC_FREE_WORK; }
#endif

//------------------------------------------------------------------------------
// DYC_CLEAR_MSG: clear the error msg string
//------------------------------------------------------------------------------

#define DYC_CLEAR_MSG                   \
    {                                   \
        if (msg != NULL) msg[0] = '\0'; \
    }

//------------------------------------------------------------------------------
// DYC_ERROR_MSG: set the error msg string
//------------------------------------------------------------------------------

#define DYC_ERROR_MSG(...)                           \
    {                                                \
        if (msg != NULL && msg[0] == '\0') {         \
            snprintf(msg, DYC_MSG_LEN, __VA_ARGS__); \
        }                                            \
    }

//------------------------------------------------------------------------------
// DYC_ASSERT_MSGF: assert an expression is true, and return if it is false
//------------------------------------------------------------------------------

#define DYC_ASSERT_MSGF(expression, error_status, expression_format, ...) \
    {                                                                     \
        if (!(expression)) {                                              \
            DYC_ERROR_MSG(                                                \
                "DynamicClosure failure (file %s, line "                  \
                "%d): " expression_format,                                \
                __FILE__, __LINE__, __VA_ARGS__);                         \
            DYC_FREE_ALL;                                                 \
            return (error_status);                                        \
        }                                                                 \
    }

//------------------------------------------------------------------------------
// DYC_ASSERT_MSG: assert an expression is true, and return if it is false
//------------------------------------------------------------------------------

// Identical to DYC_ASSERT, except this allows a different string to be
// included in the message.

#define DYC_ASSERT_MSG(expression, error_status, expression_message) \
    DYC_ASSERT_MSGF(expression, error_status, "%s", expression_message)

//------------------------------------------------------------------------------
// DYC_ASSERT: assert an expression is true, and return if it is false
//------------------------------------------------------------------------------

#define DYC_ASSERT(expression, error_status)                                 \
    {                                                                        \
        if (!(expression)) {                                                 \
            DYC_ERROR_MSG(                                                   \
                "DynamicClosure assertion \"%s\" failed (file %s, line %d):" \
                " status: %d",                                               \
                #expression, __FILE__, __LINE__, error_status);              \
            DYC_FREE_ALL;                                                    \
            return (error_status);                                           \
        }                                                                    \
    }

//------------------------------------------------------------------------------
// DYC_CATCH: catch an error from GraphBLAS
//------------------------------------------------------------------------------

#ifndef DYC_CATCH
#define DYC_CATCH(info)                                                      \
    {                                                                        \
        DYC_ERROR_MSG("DynamicClosure failure (file %s, line %d): info: %d", \
            __FILE__, __LINE__, info);                                       \
        DYC_FREE_ALL;                                                        \
        return (info);                                                       \
    }
#endif

//------------------------------------------------------------------------------
// DYC_TRY: assert an expression is true, and return if it is false
//------------------------------------------------------------------------------

#ifndef NDEBUG
#define DYC_TRY(DynamicClosure_Method)                     \
    {                                                      \
        int DynamicClosure_Status = DynamicClosure_Method; \
        if (DynamicClosure_Status < GrB_SUCCESS) {         \
            DYC_CATCH(DynamicClosure_Status);              \
        }                                                  \
    }
#else
#define DYC_TRY(DynamicClosure_Method) DynamicClosure_Method
#endif