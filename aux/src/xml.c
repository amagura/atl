/* vim: ts=5:sts=5:sw=5:set expandtab: */
#define _POSIX_C_SOURCE 1
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <libxml/xmlmemory.h>
#define COMNR_DEBUG 0
#define PROGNAME "mkinfo"
#define COMNR_PROGNAME "mkinfo"
#include <commoner.h>
#include <time.h>
#include <unistd.h>

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include "mkinfo.h"

#include "xml.h"

xmlDocPtr xopen(char *file)
{
     xmlDocPtr xp = xmlParseFile(file);
     if (xp != NULL)
          return xp;
     return NULL;
}

void xclose(xmlDocPtr doc)
{
     xmlFreeDoc(doc);
     xmlCleanupParser();
}

void *xfree(void *ptr)
{
     xmlFree(ptr);
     ptr = NULL;
     return ptr;
}

xmlXPathObjectPtr getnode(xmlDocPtr xp, xmlChar *xpath)
{
     COMNR_DBG("xpath: '%s'\n", (char *)xpath);
     xmlXPathContextPtr cntxt;
     xmlXPathObjectPtr result;
     cntxt = xmlXPathNewContext(xp);

     if (cntxt == NULL) {
          comnr_log("error in xmlXPathNewContext");
          return NULL;
     }

     result = xmlXPathEvalExpression(xpath, cntxt);
     xmlXPathFreeContext(cntxt);

     if (result == NULL) {
          comnr_log("error in xmlXPathEvalExpression");
          return NULL;
     }

     if (xmlXPathNodeSetIsEmpty(result->nodesetval)) {
          xmlXPathFreeObject(result);
          COMNR_DBG("no result could be found for node: `%s'", xpath);
          return NULL;
     }

     return result;
}

xmlXPathObjectPtr
getnodeset (xmlDocPtr doc, xmlChar *xpath)
{
     xmlXPathContextPtr context;
     xmlXPathObjectPtr result;

     context = xmlXPathNewContext(doc);

     if (context == NULL) {
          printf("Error in xmlXPathNewContext\n");
          return NULL;
     }

     result = xmlXPathEvalExpression(xpath, context);
     xmlXPathFreeContext(context);

     if (result == NULL) {
          printf("Error in xmlXPathEvalExpression\n");
          return NULL;
     }

     if (xmlXPathNodeSetIsEmpty(result->nodesetval)) {
          xmlXPathFreeObject(result);
          printf("No result\n");
          return NULL;
     }

     return result;
}


/* this is going to be a fucking huge function */
int modxml(struct changes ch)
{
     if (ch.dry)
          return -1;

     char *tmp = NULL;
     xmlDocPtr doc = xopen(ch.xml);
     COMNR_DBG("frst.xpath: '%s'\n", ch.xpath);
     COMNR_DBG("frst.value: '%s'\n", ch.value);
     xmlXPathContextPtr xpath_ctx = NULL;
     xmlXPathObjectPtr xpath_obj = NULL;
     xmlChar *xp = memdup((char *)ch.xpath, 24);
     xmlXPathObjectPtr xresult = getnodeset(doc, xp);

     /* create a temporary file and open it for writing */
     char *template = malloc(BTL_TMPSIZE + 1);
     strcpy(template, BTL_TMPLATE);
     int tmpfd = mkstmp(template);
     /* catches an error that occurs when mkstmp
      * can't create a temporary file
      */
     assert(tmpfd != -1);
     FILE *tmpfp = fdopen(tmpfd, "w+");
     COMNR_DBG("temporary file: '%s'\n", template);

     /*fprintf(stderr, "%s  %s\n", "\n***", "Original XML");*/
     /*xmlSaveFormatFile("/dev/stderr", doc, 1);*/
     /*fprintf(stderr, "%s\n", "***  End");*/

     /* select all target nodes */
     if (xresult) {
          xmlNodeSetPtr xnode = xresult->nodesetval;
          xmlNodeSetContent(xnode->nodeTab[ch.chid], (const xmlChar *)ch.value);
          time_t rawtime;
          struct tm *ltimep;
          time(&rawtime);
          ltimep = localtime(&rawtime);
          tmp = ptrim(asctime(ltimep));
          COMNR_DBG("(asctime) current time: '%s'\n", asctime(ltimep));
          xmlSetProp(xnode->nodeTab[ch.chid], (const xmlChar *)"timestamp", (const xmlChar *)tmp);
          free(tmp);
          xmlSetProp(xnode->nodeTab[ch.chid], (const xmlChar *)"built-by", (const xmlChar *)getlogin());
#if 0
          for (int pdx = 0; pdx < xnode->nodeNr; ++pdx) {
               COMNR_DBG("pdx: '%d'\n", pdx);
               xmlNodePtr tnode = xnode->nodeTab[pdx];
               kw = xmlNodeListGetString(doc,
                                         tnode->xmlChildrenNode,
                                         1);
               COMNR_DBG("(kw) Before update: '%s'\n", kw);
               xfree(kw);
               COMNR_DBG("ch.frst->value: '%s'\n", ch.frst->value);
               xmlNodeSetContent(xnode->nodeTab[pdx], ch.frst->value);
               kw = xmlNodeListGetString(doc,
                                         tnode->xmlChildrenNode,
                                         1);
               COMNR_DBG("(kw) After update: '%s'\n", kw);
               xfree(kw);
          }
#endif
          xmlXPathFreeObject(xresult);
          xmlDocDump(tmpfp, doc);
     }
     xfree(xp);

     /*[> create xpath evaluation context <]*/
     /*xpath_ctx = xmlXPathNewContext(doc);*/
     /*if (xpath_ctx == NULL) {*/
          /*return EXML_XPATHCTX;*/
     /*}*/

     /*[> evaluate xpath expression <]*/
     /*xpath_obj = xmlXPathEvalExpression(ch.xpath, xpath_ctx);*/
     /*if (xpath_obj == NULL) {*/
          /*xmlXPathFreeContext(xpath_ctx);*/
          /*return EXML_XPATHEV;*/
     /*}*/

     xmlXPathFreeObject(xpath_obj);
     xmlXPathFreeContext(xpath_ctx);

     close(tmpfd);
     fclose(tmpfp);
     if (ch.deploy) {
          /* dump temporary file to original xml file */
          xclose(doc);
          doc = xopen(template);
          FILE *fp = NULL;
          if ((fp = fopen(ch.xml, "w")) == NULL) {
               printf("%s: could not open file %s: %s\n", PROGNAME, ch.xml, strerror(errno));
               /*printf("%s: could not open file %s\n", PROGNAME, ch.xml);*/
               exit(EXIT_FAILURE);
          }
          xmlDocDump(fp, doc);
          fclose(fp);
          xclose(doc);
          unlink(template); // delete template file
     } else {
          printf("%s\n", template);
     }
     free(template);
     return 0;
}

char **get_xpath(char *xml, xmlChar *xpath, int *cnt)
{
     bool freed = true;
     if (cnt == NULL) {
          cnt = malloc(sizeof(int));
          *cnt = 1;
          freed = false;
     }
    COMNR_DBG("xpath: '%s'\n", (char *)xpath);
    COMNR_DBG("xml: '%s'\n", xml);
    xmlXPathObjectPtr result;
    xmlNodeSetPtr nodeset;
    xmlChar *kw;

    xmlDocPtr doc = xopen(xml);

    // Nodes are
    // parse the xml document and find those nodes that meet the criteria of the xpath.
    result = getnode(doc, xpath);

    // if it parsed and found anything
    if (result)
    {
        // get the nodes that matched.
        nodeset = result->nodesetval;
        // go through each Node. There are nodeNr number of nodes.
        // nodeset is the seta of all nodes that met the xpath criteria
        // For the API look here http://xmlsoft.org/html/libxml-xpath.html

        char **v = malloc(2 * 32);
        char **wp = v;

        for (int mdx = 0; mdx < *cnt; ++mdx, ++wp) {
             kw = xmlNodeListGetString(doc, nodeset->nodeTab[mdx]->xmlChildrenNode, 1);
             COMNR_DBG("kw: '%s'\n", kw);
             COMNR_DBG("kw mem addr: '%p'\n", kw);
             if (!kw) {
                  *wp = NULL;
                  continue;
             }
             size_t kwsz = strlen((char *)kw) + 1;
             COMNR_DBG("size of kw: '%lu'\n", kwsz);
             *wp = malloc(kwsz);
             memcpy(*wp, kw, kwsz);
             kw = xfree(kw);
             COMNR_DBG("(%d) *wp: '%s'\n", mdx, *wp);
             COMNR_DBG("(%d) *wp addr: '%p'\n", mdx, *wp);
        }
        xmlXPathFreeObject(result);
        comnr_pong;
        COMNR_DBG("v adrr: '%p'\n", v);
        if (!freed)
             free(cnt);
        xclose(doc);
        return v;
    }
    if (!freed)
         free(cnt);
    xclose(doc);
    return NULL;
}

char **getv_xpath(xmlDocPtr doc, xmlChar *xpath)
{
    COMNR_DBG("xpath: '%s'\n", (char *)xpath);
    xmlXPathObjectPtr result;
    xmlNodeSetPtr nodeset;
    xmlChar *kw;

    // Nodes are
    // parse the xml document and find those nodes that meet the criteria of the xpath.
    result = getnode(doc, xpath);

    // if it parsed and found anything
    if (result)
    {
        // get the nodes that matched.
        nodeset = result->nodesetval;
        // go through each Node. There are nodeNr number of nodes.
        // nodeset is the seta of all nodes that met the xpath criteria
        // For the API look here http://xmlsoft.org/html/libxml-xpath.html

        char **v = malloc(2 * 32);
        char **wp = v;

        for (int mdx = 0; mdx < 2; ++mdx, ++wp) {
             kw = xmlNodeListGetString(doc, nodeset->nodeTab[mdx]->xmlChildrenNode, 1);
             COMNR_DBG("kw: '%s'\n", kw);
             COMNR_DBG("kw mem addr: '%p'\n", kw);
             if (!kw) {
                  *wp = NULL;
                  continue;
             }
             size_t kwsz = strlen((char *)kw) + 1;
             COMNR_DBG("size of kw: '%lu'\n", kwsz);
             *wp = malloc(kwsz);
             memcpy(*wp, kw, kwsz);
             kw = xfree(kw);
             COMNR_DBG("(%d) *wp: '%s'\n", mdx, *wp);
             COMNR_DBG("(%d) *wp addr: '%p'\n", mdx, *wp);
        }
        xmlXPathFreeObject(result);
        comnr_pong;
        COMNR_DBG("v adrr: '%p'\n", v);
        return v;
    }
    return NULL;
}
