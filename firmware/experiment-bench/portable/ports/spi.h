// SPDX-FileCopyrightText: 2026 Companion Platform contributors
//
// SPDX-License-Identifier: Apache-2.0

// Ports SPI EXPLICITES : le role MAITRE et le role ESCLAVE n'ont PAS le meme
// cycle de vie et ne partagent donc PAS une meme primitive synchrone.
//
//  - Le MAITRE (role Host) declenche l'echange : transfer/cancel/status.
//  - L'ESCLAVE (module CX-Bus) est PASSIF, pilote par CS/maitre : il prepare une
//    reponse, recoit une selection, consomme la transaction, signale son etat.
//
// Ces contrats sont declares cote portable (le coeur en depend) ; les
// adaptateurs (hal/<cible>/) les IMPLEMENTENT. Le coeur n'inclut jamais de
// dependance plateforme.
#ifndef BENCH_PORT_SPI_H
#define BENCH_PORT_SPI_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
  BENCH_SPI_OK = 0,       // echange termine
  BENCH_SPI_BUSY = 1,     // echange en cours (asynchrone)
  BENCH_SPI_TIMEOUT = 2,  // aucune reponse dans le budget imparti
  BENCH_SPI_ERROR = 3,    // erreur bus / arguments
} bench_spi_status_t;

// --- Port MAITRE (Host) ---------------------------------------------------
// transfer() realise un echange full-duplex de "len" octets (tx -> peripherique,
// peripherique -> rx). status() renseigne l'etat courant ; cancel() abandonne un
// echange en cours.
typedef struct bench_spi_master {
  void *ctx;
  bench_spi_status_t (*transfer)(void *ctx, const uint8_t *tx, uint8_t *rx,
                                 size_t len);
  void (*cancel)(void *ctx);
  bench_spi_status_t (*status)(void *ctx);
} bench_spi_master_t;

typedef enum {
  BENCH_SLAVE_IDLE = 0,      // aucune transaction
  BENCH_SLAVE_SELECTED = 1,  // CS asserte, en attente d'octets
  BENCH_SLAVE_ACTIVE = 2,    // transaction en cours
  BENCH_SLAVE_COMPLETE = 3,  // transaction consommee, reponse prete
} bench_slave_txn_status_t;

// --- Port ESCLAVE (module CX-Bus) -----------------------------------------
// prepare_response() charge les octets a restituer lors de la PROCHAINE
// transaction. on_select() est appele quand le maitre asserte CS. on_transaction()
// consomme les octets recus et retourne le nombre effectivement consomme.
// transaction_status() expose l'etat courant.
// peek_response() expose les octets que l'esclave restituera (les octets MISO
// que le maitre lira pendant l'echange) ; c'est l'image de la FIFO d'emission
// chargee par prepare_response(). Le "milieu" (bus reel ou lien simule) s'en
// sert pour cadencer les octets vers le maitre.
typedef struct bench_spi_slave {
  void *ctx;
  void (*prepare_response)(void *ctx, const uint8_t *resp, size_t len);
  void (*on_select)(void *ctx);
  size_t (*on_transaction)(void *ctx, const uint8_t *rx, size_t len);
  size_t (*peek_response)(void *ctx, uint8_t *out, size_t cap);
  bench_slave_txn_status_t (*transaction_status)(void *ctx);
} bench_spi_slave_t;

#endif  // BENCH_PORT_SPI_H
