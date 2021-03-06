/* Copyright (c) 2014 SRI International and Suns-tech Incorporated
 * Developed under DARPA contract N66001-11-C-4022.
 * Authors:
 *   Sam Wood (SW)
 */

#include "ForwardingClassifierPriority.h"

#include "ForwardingClassifierFactory.h"

ForwardingClassifierPriority::ForwardingClassifierPriority(ForwardingManager *m)  :
    ForwardingClassifier(m, FORWARDING_CLASSIFIER_PRIORITY_NAME), 
    initialized(false),
    kernel(getManager()->getKernel())
{

}

ForwardingClassifierPriority::~ForwardingClassifierPriority() 
{
    while (!classifierList.empty()) {
        ForwardingClassifier *classifier = (*classifierList.begin()).first;
        if (NULL == classifier) {
            HAGGLE_ERR("Null classifier in registry\n");
        }
        classifierList.pop_front();
        delete classifier;
    }
}

const string
ForwardingClassifierPriority::getClassNameForDataObject(
    const DataObjectRef& dObj)
{
    if (!initialized) {
        HAGGLE_ERR("Classifier has not been fully initialized.\n");
        return FORWARDING_CLASSIFIER_INVALID_CLASS;
    }

    if (!dObj) {
        HAGGLE_ERR("Received null data object.\n");
        return FORWARDING_CLASSIFIER_INVALID_CLASS;
    }

    int bestPriority = -1;
    string bestTag = FORWARDING_CLASSIFIER_INVALID_CLASS;
    for (ClassifierListType::const_iterator it = classifierList.begin(); 
         it != classifierList.end(); it++) {
        ForwardingClassifier *classifier = (*it).first;
        int priority = (*it).second;
        if (priority <= bestPriority) {
            continue;
        }

        string currentTag = classifier->getClassNameForDataObject(dObj);
        if (FORWARDING_CLASSIFIER_INVALID_CLASS != currentTag) {
            bestPriority = priority;
            bestTag = currentTag;
        }
    }

    HAGGLE_DBG("Best tag: %s, priority: %d\n", bestTag.c_str(), bestPriority);

    return bestTag;
}

void 
ForwardingClassifierPriority::onForwardingClassifierConfig(
    const Metadata& m)
{
   initialized = false;

    if (0 != strcmp(getName(), m.getName().c_str())) {
        HAGGLE_ERR("Invalid metadata to config handler.\n");
        return;
    }

    const char *param = NULL;
    int numClassifiers = 0;

    const Metadata *classifierConfig;
    int i = 0;
    int paramInt; 
    char *endptr = NULL;
    
    while (NULL != (classifierConfig = m.getMetadata("ForwardingClassifier", i++))) {

        param = classifierConfig->getParameter(FORWARDING_CLASSIFIER_PRIORITY_CHILD_NAME);

        if (NULL == param) {
            HAGGLE_ERR("No classifier specified\n");
            return;
        }
        
        string classifierName = string(param);

        param = classifierConfig->getParameter(FORWARDING_CLASSIFIER_PRIORITY_CHILD_PRIORITY_NAME);
        if (NULL == param) {
            HAGGLE_ERR("No priority specified: %s.\n", classifierName.c_str());
            return;
        }

        int priority;
        endptr = NULL;
        paramInt = (int) strtol(param, &endptr, 10);
        if (endptr && endptr != param && paramInt > 0) {
            priority = paramInt;
        } 
        else {
            HAGGLE_ERR("Problems parsing priority.\n");
            return;
        }
        
        ForwardingClassifier *classifier = ForwardingClassifierFactory::getClassifierForName(getManager(), classifierName);

        if (NULL == classifier) {
            HAGGLE_ERR("Classifier priority could not instantiate classifier: %s\n", classifierName.c_str());
            return;
        } 

        const Metadata* classifierConfigOptions = classifierConfig->getMetadata(classifierName);
        if (NULL == classifierConfigOptions) {
            HAGGLE_ERR("No classifier config option set\n");
            return;
        }

        classifier->onConfig(*classifierConfig);

        classifierList.push_back(PriorityPairType(classifier, priority));
        HAGGLE_DBG("Priority classifier added classifier: %s, priority: %d\n", classifierName.c_str(), priority);
        numClassifiers++;
    }

    HAGGLE_DBG("Initialized priority protocol classifier with %d classifiers\n", numClassifiers);

    initialized = true;
}
