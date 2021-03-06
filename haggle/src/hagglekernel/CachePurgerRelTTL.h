/* Copyright (c) 2014 SRI International and Suns-tech Incorporated
 * Developed under DARPA contract N66001-11-C-4022.
 * Authors:
 *   Sam Wood (SW)
 *   James Mathewson (JM, JLM)
 */

#ifndef _CACHE_PURGER_RELTTL_H
#define _CACHE_PURGER_RELTTL_H

class CachePurgerRelTTL;

#include "ManagerModule.h"
#include "DataManager.h"
#include "Metadata.h"
#include "CachePurger.h"

#define CACHE_PURGER_REL_TTL_NAME "CachePurgerRelTTL"
#define CACHE_PURGER_REL_TTL_METRIC "purge_type"
#define CACHE_PURGER_REL_TTL_TAG "tag_field"
#define CACHE_PURGER_REL_TTL_TAG_VALUE "tag_field_value"
#define CACHE_PURGER_REL_TTL_KEEP_IN_BF_NAME "keep_in_bloomfilter"
#define CACHE_PURGER_REL_TTL_MIN_DB_TIME_S "min_db_time_seconds"
#define CACHE_PURGER_REL_TTL_MIN_DB_S (2.0)
#define CACHE_PURGER_REL_TTL_KEEP_IN_BF (false)

class CachePurgerRelTTLCallback;

class CachePurgerRelTTL : public CachePurger {
private:
    bool keepInBF;
    double minDBtimeS;
    string metricField;
    string tagField;
    string tagFieldValue;
    CachePurgerRelTTLCallback *purgerCallback;
    EventCallback<EventHandler> *initCallback;
    EventCallback<EventHandler> *deleteCallback;
public:
    CachePurgerRelTTL(DataManager *m = NULL);

    ~CachePurgerRelTTL();

    void onConfig(const Metadata& m);

    bool isResponsibleForDataObject(DataObjectRef &dObj);

    void schedulePurge(DataObjectRef &dObj);

    void initializationPurge();

    double getMinDBTimeS() { return minDBtimeS; }

    string getMetricField() { return metricField; }

    string getTagField() { return tagField; }

    string getTagFieldValue() { return tagFieldValue; }

    bool getKeepInBloomfilter() { return keepInBF; }
};

#endif /* _CACHE_PURGER_RELTTL_H */
