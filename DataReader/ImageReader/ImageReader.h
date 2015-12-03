//
// <copyright company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//

#pragma once
#include <random>
#include <memory>
#include <future>
#include <array>
#include "DataReader.h"

namespace Microsoft { namespace MSR { namespace CNTK {

// REVIEW alexeyk: can't put it into ImageReader itself as ImageReader is a template.
class ITransform;

template<class ElemType>
class ImageReader : public IDataReader<ElemType>
{
public:
    ImageReader();
    virtual ~ImageReader();

    ImageReader(const ImageReader&) = delete;
    ImageReader& operator=(const ImageReader&) = delete;
    ImageReader(ImageReader&&) = delete;
    ImageReader& operator=(ImageReader&&) = delete;

    void Init(const ConfigParameters& config) override;
    void Destroy() override;
    void StartMinibatchLoop(size_t mbSize, size_t epoch, size_t requestedEpochSamples = requestDataSize) override;
    bool GetMinibatch(std::map<std::wstring, Matrix<ElemType>*>& matrices) override;
    bool DataEnd(EndDataType endDataType) override;

    size_t GetNumParallelSequences() override  { return m_pMBLayout->GetNumParallelSequences(); }
    void CopyMBLayoutTo(MBLayoutPtr pMBLayout) override { pMBLayout->CopyFrom(m_pMBLayout); };

    void SetRandomSeed(unsigned int seed) override;

private:
    // Mini-batch format: NCHW (default, cuDNN) or NHWC (legacy).
    enum class DataFormat { NCHW, NHWC };

    unsigned int m_seed;
    std::mt19937 m_rng;

    std::vector<std::unique_ptr<ITransform>> m_transforms;

    std::wstring m_featName;
    std::wstring m_labName;

    size_t m_featDim;
    size_t m_labDim;

    using StrIntPairT = std::pair<std::string, int>;
    std::vector<StrIntPairT> m_files;

    size_t m_epochSize;
    size_t m_mbSize;
    size_t m_epoch;

    size_t m_epochStart;
    size_t m_mbStart;

    //std::vector<ElemType> m_featBuf;
    //std::vector<ElemType> m_labBuf;

    std::vector<std::future<void>> m_mbFut;
    int m_mbIdx;
    std::array<std::vector<ElemType>, 2> m_featBuf;
    std::array<std::vector<ElemType>, 2> m_labBuf;

    bool m_imgListRand;

    MBLayoutPtr m_pMBLayout;
    DataFormat m_mbFmt;

private:
    void Prefetch();
    void ReadImage(size_t idxFile, size_t idxInMB);
};
}}}
