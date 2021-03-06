/*------------------------------------------------------------------------------------ */
/* Implementation of the simple pipe ECHO hash function in its 512-bit outputs variant.*/
/* Optimized for Pentium, legacy x86                                                   */
/*                                                                                     */
/* Date:     2010-07-23                                                                */
/*                                                                                     */
/* Authors:  Ryad Benadjila  <ryadbenadjila@gmail.com>                                 */
/*           Olivier Billet  <billet@eurecom.fr>                                       */
/*------------------------------------------------------------------------------------ */
#define CRYPTO_BYTES 64
#define CRYPTO_VERSION "2.1"
