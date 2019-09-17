/* DZco - digital control library
 * Copyright (C) 2000 Tomomichi Sugihara (Zhidao)
 */
/*! \file dz_errmsg.h
 * \brief error messages.
 * \author Zhidao
 */

#ifndef __DZ_ERRMSG_H__
#define __DZ_ERRMSG_H__

/* NOTE: never include this header file in user programs. */

/* warning message */

#define DZ_WARN_SYS_INVALID_OUTPUTPORT "invalid output port of a system %s:%d"
#define DZ_WARN_SYS_INVALID_INPUTPORT  "invalid input port of a system %s:%d"
#define DZ_WARN_SYS_TYPE_UNFOUND       "cannot find a system type %s"
#define DZ_WARN_SYS_NAME_UNFOUND       "cannot find a system name %s"
#define DZ_WARN_SYS_ALREADYCONNECTED   "connection already determined, invalid token %s"

#define DZ_WARN_SYSARRAY_EMPTY         "empty array of systems specified."

/* error messages */

#define DZ_ERR_TF_UNABLE_CREATE        "cannot create a transfer function"
#define DZ_ERR_TF_INVALID_DEN          "invalid denominator"
#define DZ_ERR_TF_NONPROPER            "non-proper system"

#define DZ_ERR_LIN_UNCTRL              "system is not controllable"
#define DZ_ERR_LIN_UNASSIGNABLE_POLE   "cannot assign desired poles"
#define DZ_ERR_LIN_UNCONVERTIBLE_TF    "cannot convert a transfer function to linear system"
#define DZ_ERR_LIN_SIZMIS              "size mismatch of system matrices"

#define DZ_ERR_IDENT_LAG_UNTRIGERRED   "trigger not found"

#define DZ_ERR_SYS_TYPE_UNSPECIFIED    "type not specified"

#define DZ_ERR_SYS_TF_UNABLE_CONV      "unable to convert a linear system to a transfer function"

#define DZ_ERR_SYS_PID_NEGATIVEFGT     "negative forgetting factor %g specified"
#define DZ_ERR_SYS_PID_TOOLARGEFGT     "forgetting factor %g larger than 1 specified"

#define DZ_ERR_SYS_LAG_TOOSHORTTC      "too short time constant"

#define DZ_ERR_SYS_BW_ZEROORDER        "cannot create a zero-order filter"

#define DZ_ERR_FATAL                   "fatal error! - please report to the author"

#endif /* __DZ_ERRMSG_H__ */
