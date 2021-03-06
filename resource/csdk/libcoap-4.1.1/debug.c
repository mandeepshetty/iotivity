/* debug.c -- debug utilities
 *
 * Copyright (C) 2010--2012 Olaf Bergmann <bergmann@tzi.org>
 *
 * This file is part of the CoAP library libcoap. Please see
 * README for terms of use.
 */

#include "config.h"

#if defined(HAVE_ASSERT_H) && !defined(assert)
# include <assert.h>
#endif

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#ifdef HAVE_TIME_H
#include <time.h>
#endif

#include "debug.h"
#include "net.h"

#ifdef WITH_CONTIKI
# ifndef DEBUG
#  define DEBUG DEBUG_PRINT
# endif /* DEBUG */
#include "net/uip-debug.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

static coap_log_t maxlog = LOG_WARNING;	/* default maximum log level */

coap_log_t
coap_get_log_level() {
  return maxlog;
}

void
coap_set_log_level(coap_log_t level) {
  maxlog = level;
}

/* this array has the same order as the type log_t */
static char *loglevels[] = {
  "EMRG", "ALRT", "CRIT", "ERR", "WARN", "NOTE", "INFO", "DEBG"
};

#ifdef HAVE_TIME_H

static inline size_t
print_timestamp(char *s, size_t len, coap_tick_t t) {
  struct tm *tmp;
  time_t now = clock_offset + (t / COAP_TICKS_PER_SECOND);
  tmp = localtime(&now);
  return strftime(s, len, "%b %d %H:%M:%S", tmp);
}

#else /* alternative implementation: just print the timestamp */

static inline size_t
print_timestamp(char *s, size_t len, coap_tick_t t) {
#ifdef HAVE_SNPRINTF
  return snprintf(s, len, "%u.%03u",
		  (unsigned int)(clock_offset + (t / COAP_TICKS_PER_SECOND)),
		  (unsigned int)(t % COAP_TICKS_PER_SECOND));
#else /* HAVE_SNPRINTF */
  /* @todo do manual conversion of timestamp */
  return 0;
#endif /* HAVE_SNPRINTF */
}

#endif /* HAVE_TIME_H */

#ifndef NDEBUG

#ifndef HAVE_STRNLEN
/**
 * A length-safe strlen() fake.
 *
 * @param s      The string to count characters != 0.
 * @param maxlen The maximum length of @p s.
 *
 * @return The length of @p s.
 */
static inline size_t
strnlen(const char *s, size_t maxlen) {
  size_t n = 0;
  while(*s++ && n < maxlen)
    ++n;
  return n;
}
#endif /* HAVE_STRNLEN */

unsigned int
print_readable( const unsigned char *data, unsigned int len,
		unsigned char *result, unsigned int buflen, int encode_always ) {
  const unsigned char hex[] = "0123456789ABCDEF";
  unsigned int cnt = 0;
  assert(data || len == 0);

  if (buflen == 0 || len == 0)
    return 0;

  while (len) {
    if (!encode_always && isprint(*data)) {
      if (cnt == buflen)
	break;
      *result++ = *data;
      ++cnt;
    } else {
      if (cnt+4 < buflen) {
	*result++ = '\\';
	*result++ = 'x';
	*result++ = hex[(*data & 0xf0) >> 4];
	*result++ = hex[*data & 0x0f];
	cnt += 4;
      } else
	break;
    }

    ++data; --len;
  }

  *result = '\0';
  return cnt;
}

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

size_t
coap_print_addr(const coap_address_t *addr, unsigned char *buf, size_t len) {
TODO(FIX: Fix this)
    (void)addr;
    (void)buf;
    (void)len;
    return 0;
}

#ifndef WITH_CONTIKI
void
coap_show_pdu(const coap_pdu_t *pdu) {
  unsigned char buf[COAP_MAX_PDU_SIZE]; /* need some space for output creation */
  int encode = 0, have_options = 0;
  coap_opt_iterator_t opt_iter;
  coap_opt_t *option;

  fprintf(COAP_DEBUG_FD, "v:%d t:%d tkl:%d c:%d id:%u",
	  pdu->hdr->version, pdu->hdr->type,
	  pdu->hdr->token_length,
	  pdu->hdr->code, ntohs(pdu->hdr->id));

  /* show options, if any */
  coap_option_iterator_init((coap_pdu_t *)pdu, &opt_iter, COAP_OPT_ALL);

  while ((option = coap_option_next(&opt_iter))) {
    if (!have_options) {
      have_options = 1;
      fprintf(COAP_DEBUG_FD, " o: [");
    } else {
      fprintf(COAP_DEBUG_FD, ",");
    }

    if (opt_iter.type == COAP_OPTION_URI_PATH ||
	opt_iter.type == COAP_OPTION_PROXY_URI ||
	opt_iter.type == COAP_OPTION_URI_HOST ||
	opt_iter.type == COAP_OPTION_LOCATION_PATH ||
	opt_iter.type == COAP_OPTION_LOCATION_QUERY ||
    opt_iter.type == COAP_OPTION_URI_PATH ||
    opt_iter.type == COAP_OPTION_OBSERVE ||
	opt_iter.type == COAP_OPTION_URI_QUERY) {
      encode = 0;
    } else {
      encode = 1;
    }

    if (print_readable(COAP_OPT_VALUE(option),
		       COAP_OPT_LENGTH(option),
		       buf, sizeof(buf), encode ))
      fprintf(COAP_DEBUG_FD, " %d:'%s'", opt_iter.type, buf);
  }

  if (have_options)
    fprintf(COAP_DEBUG_FD, " ]");

  if (pdu->data) {
    assert(pdu->data < (unsigned char *)pdu->hdr + pdu->length);
    print_readable(pdu->data,
		   (unsigned char *)pdu->hdr + pdu->length - pdu->data,
		   buf, sizeof(buf), 0 );
    fprintf(COAP_DEBUG_FD, " d:%s", buf);
  }
  fprintf(COAP_DEBUG_FD, "\n");
  fflush(COAP_DEBUG_FD);
}

#else /* WITH_CONTIKI */

void
coap_show_pdu(const coap_pdu_t *pdu) {
  unsigned char buf[80]; /* need some space for output creation */

  PRINTF("v:%d t:%d oc:%d c:%d id:%u",
	  pdu->hdr->version, pdu->hdr->type,
	  pdu->hdr->optcnt, pdu->hdr->code, uip_ntohs(pdu->hdr->id));

  /* show options, if any */
  if (pdu->hdr->optcnt) {
    coap_opt_iterator_t opt_iter;
    coap_opt_t *option;
    coap_option_iterator_init((coap_pdu_t *)pdu, &opt_iter, COAP_OPT_ALL);

    PRINTF(" o:");
    while ((option = coap_option_next(&opt_iter))) {

      if (print_readable(COAP_OPT_VALUE(option),
			 COAP_OPT_LENGTH(option),
			 buf, sizeof(buf), 0))
	PRINTF(" %d:%s", opt_iter.type, buf);
    }
  }

  if (pdu->data < (unsigned char *)pdu->hdr + pdu->length) {
    print_readable(pdu->data,
		   (unsigned char *)pdu->hdr + pdu->length - pdu->data,
		   buf, sizeof(buf), 0 );
    PRINTF(" d:%s", buf);
  }
  PRINTF("\r\n");
}
#endif /* WITH_CONTIKI */

#endif /* NDEBUG */

#ifndef WITH_CONTIKI
void
coap_log_impl(coap_log_t level, const char *format, ...) {
  char timebuf[32];
  coap_tick_t now;
  va_list ap;
  FILE *log_fd;

  if (maxlog < level)
    return;

  log_fd = level <= LOG_CRIT ? COAP_ERR_FD : COAP_DEBUG_FD;

  coap_ticks(&now);
  if (print_timestamp(timebuf,sizeof(timebuf), now))
    fprintf(log_fd, "%s ", timebuf);

  if (level <= LOG_DEBUG)
    fprintf(log_fd, "%s ", loglevels[level]);

  va_start(ap, format);
  vfprintf(log_fd, format, ap);
  va_end(ap);
  fflush(log_fd);
}
#else /* WITH_CONTIKI */
void
coap_log_impl(coap_log_t level, const char *format, ...) {
  char timebuf[32];
  coap_tick_t now;
  va_list ap;

  if (maxlog < level)
    return;

  coap_ticks(&now);
  if (print_timestamp(timebuf,sizeof(timebuf), now))
    PRINTF("%s ", timebuf);

  if (level <= LOG_DEBUG)
    PRINTF("%s ", loglevels[level]);

  va_start(ap, format);
  PRINTF(format, ap);
  va_end(ap);
}
#endif /* WITH_CONTIKI */
#ifdef __cplusplus
}
#endif
