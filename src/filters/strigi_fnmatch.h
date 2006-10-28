#if defined(HAVE_FNMATCH_H)

#elif !defined(STRIGI_FNMATCH_H_)
#define STRIGI_FNMATCH_H


    #define FNM_NOMATCH     1     /**< Match failed. */
    #define FNM_NOESCAPE    0x01  /**< Disable backslash escaping. */
    #define FNM_PATHNAME    0x02  /**< Slash must be matched by slash. */
    #define FNM_PERIOD      0x04  /**< Period must be matched by period. */
    #define FNM_CASE_BLIND  0x08  /**< Compare characters case-insensitively.
                                       * @remark This flag is an Apache addition 
                                       */
    #define SUCCESS 0

    extern int fnmatch(const char *pattern, const char *string, int flags);
#endif
