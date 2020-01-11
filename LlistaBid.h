#ifndef _LLISTABID_
#define _LLISTABID_

#include <stdio.h>
#include <stdlib.h>

#include "DataStruct.h"


LlistaBid LLISTABID_crea();
int LLISTABID_inici(LlistaBid l);
int LLISTABID_fi (LlistaBid l);
int LLISTABID_esBuida (LlistaBid l);
void LLISTABID_vesInici (LlistaBid* l);
void LLISTABID_vesFinal (LlistaBid* l);
void LLISTABID_avanca (LlistaBid* l);
void LLISTABID_retrocedeix(LlistaBid *l);
void LLISTABID_inserir (LlistaBid* l, connectionInfo ci);
//void LLISTABID_insereixOrdenat(LlistaBid *l, int num);
connectionInfo LLISTABID_consulta (LlistaBid l);
void LLISTABID_esborra (LlistaBid* l);
void LLISTABID_destrueix(LlistaBid *l);

#endif
