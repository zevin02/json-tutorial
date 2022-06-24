#include "leptjson.h"
#include <assert.h> /* assert() */
#include <stdlib.h> /* NULL, strtod() */

#define EXPECT(c, ch)             \
    do                            \
    {                             \
        assert(*c->json == (ch)); \
        c->json++;                \
    } while (0)

#define EXPECTED(c, ch)       \
    do                        \
    {                         \
        if (*c->json == (ch)) \
            ;                 \
        c->json++;            \
    } while (0)

typedef struct
{
    const char *json;
} lept_context;

static void lept_parse_whitespace(lept_context *c)
{
    const char *p = c->json;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
        p++;
    c->json = p;
}


static int lept_parse_number(lept_context *c, lept_value *v)
{
    char *end;
    /* \TODO validate number */
    /*strtod是把字符串转化为double类型,n也是一个浮点数接收它的数字*,这里的end就是除去了数字外的东西*/
    v->n = strtod(c->json, &end);
    if (c->json == end)/*读取完毕之后end里面的东西就不是我们想要的东西*/
        return LEPT_PARSE_INVALID_VALUE;/*这里的东西就是非法的，*/
    c->json = end;/*走到这里说明了里面啥也没有，所以就是相同的*/
    v->type = LEPT_NUMBER;
    return LEPT_PARSE_OK;
}
static int lept_parse_tnf(lept_context *c, lept_value *v,char ch)
{
    /*把第一个字符传进去进去判断*/
    int ret = 0;
    if (ch == 't')
    {
        ret = 1;
    }
    else if (ch == 'f')
        ret = 2;
    else if (ch == 'n')
        ret = 3;
    if (ret == 1)
    {
        EXPECTED(c, 't');
        if (c->json[0] != 'r' || c->json[1] != 'u' || c->json[2] != 'e')
            return LEPT_PARSE_INVALID_VALUE;
        c->json += 3;
        v->type = LEPT_TRUE;
        return LEPT_PARSE_OK;
    }
    else if (ret == 2)
    {
        EXPECTED(c, 'f');
        if (c->json[0] != 'a' || c->json[1] != 'l' || c->json[2] != 's' || c->json[3] != 'e')
            return LEPT_PARSE_INVALID_VALUE;
        c->json += 4;
        v->type = LEPT_FALSE;
        return LEPT_PARSE_OK;
    }
    else if (ret == 3)
    {
        EXPECTED(c, 'n');
        if (c->json[0] != 'u' || c->json[1] != 'l' || c->json[2] != 'l')
            return LEPT_PARSE_INVALID_VALUE;
        c->json += 3;
        v->type = LEPT_NULL;
        return LEPT_PARSE_OK;
    }
}
static int lept_parse_value(lept_context *c, lept_value *v)
{
    char ch = *c->json;
    if (*c->json == 't' || *c->json == 'f' || *c->json == 'n')
        return lept_parse_tnf(c, v,ch);
    switch (*c->json)
    {
    case '\0':
        return LEPT_PARSE_EXPECT_VALUE;
    default:
        return lept_parse_number(c, v);
    }
}

int lept_parse(lept_value *v, const char *json)
{
    lept_context c;
    int ret;
    assert(v != NULL);
    c.json = json;
    v->type = LEPT_NULL;
    lept_parse_whitespace(&c);
    if ((ret = lept_parse_value(&c, v)) == LEPT_PARSE_OK)
    {
        lept_parse_whitespace(&c);
        if (*c.json != '\0')
        {
            v->type = LEPT_NULL;
            ret = LEPT_PARSE_ROOT_NOT_SINGULAR;
        }
    }
    return ret;
}

lept_type lept_get_type(const lept_value *v)
{
    assert(v != NULL);
    return v->type;
}

double lept_get_number(const lept_value *v)
{
    assert(v != NULL && v->type == LEPT_NUMBER);
    return v->n;
}
