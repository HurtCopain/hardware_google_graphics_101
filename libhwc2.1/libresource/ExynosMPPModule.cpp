/*
 * Copyright (C) 2019 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ExynosMPPModule.h"
#include "ExynosHWCDebug.h"
#include "ExynosResourceManager.h"
#include "ExynosPrimaryDisplayModule.h"

ExynosMPPModule::ExynosMPPModule(ExynosResourceManager* resourceManager,
        uint32_t physicalType, uint32_t logicalType, const char *name,
        uint32_t physicalIndex, uint32_t logicalIndex, uint32_t preAssignInfo)
    : ExynosMPP(resourceManager, physicalType, logicalType, name, physicalIndex, logicalIndex, preAssignInfo),
    mChipId(0x00)
{
}

ExynosMPPModule::~ExynosMPPModule()
{
}

uint32_t ExynosMPPModule::getSrcXOffsetAlign(struct exynos_image &src)
{
    uint32_t idx = getRestrictionClassification(src);
    return mSrcSizeRestrictions[idx].cropXAlign;
}

int32_t ExynosMPPModule::setColorConversionInfo()
{
    if (mAssignedDisplay == nullptr) {
        MPP_LOGE("%s: mAssignedDisplay is null", __func__);
        return -EINVAL;
    }
    if (mAssignedDisplay->mType != HWC_DISPLAY_PRIMARY)
        return NO_ERROR;

    ExynosPrimaryDisplayModule* primaryDisplay =
        (ExynosPrimaryDisplayModule*)mAssignedDisplay;

    for (size_t i = 0; i < mAssignedSources.size(); i++) {
        ExynosLayer* layer = (ExynosLayer*)mAssignedSources[i];
        AcrylicLayer* mppLayer = mSrcImgs[i].mppLayer;
        if (mppLayer == nullptr) {
            MPP_LOGE("%s: src[%zu] mppLayer is null", __func__, i);
            return -EINVAL;
        }
        if (primaryDisplay->hasDppForLayer(layer) == false) {
            MPP_LOGE("%s: src[%zu] need color conversion but there is no IDpp", __func__, i);
            return -EINVAL;
        }
        MPP_LOGD(eDebugColorManagement,
                "%s, src: 0x%8x", __func__, layer->mSrcImg.dataSpace);
        const IDisplayColorGS101::IDpp& dpp =
            primaryDisplay->getDppForLayer(layer);
        mppLayer->setLayerData((void *)&dpp,
                sizeof(IDisplayColorGS101::IDpp));
    }
    return NO_ERROR;
}
