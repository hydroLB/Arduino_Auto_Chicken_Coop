//
// Created by Leon Bouramia on 3/23/23.
//

#ifndef CHICKEN_COOP_TABLE_GENERATOR_H
#define CHICKEN_COOP_TABLE_GENERATOR_H


#include <Arduino.h>

void createTable(const String &tableName);

void insertIntoTable(const String &tableName, const String &timestamp, const String &data);


#endif //CHICKEN_COOP_TABLE_GENERATOR_H
