#include "AudioBufferManager.h"

AudioBufferManager::AudioBufferManager ()
{
}

AudioBufferManager::AudioBufferManager (unsigned int blockSize)
    : blockSize (blockSize)
{
}

AudioBufferManager::~AudioBufferManager ()
{
}

const AudioBufferManager::AudioBufferID AudioBufferManager::getFreeBuffer ()
{
    for (auto& elem : buffers)
    {
        if (elem.first.isFree == true)
            return elem.first;
    }

    // No free buffers so allocate more
    return createBuffer ();
}

void AudioBufferManager::setBlockSize (unsigned int blockSize)
{
    this->blockSize = blockSize;
    // TODO re allocate buffers?
}

void AudioBufferManager::markBufferAsFree (AudioBufferID buffer)
{
    buffer.isFree = true;
    ++numberFreeBuffers;
}

std::shared_ptr<AudioBuffer<float>> AudioBufferManager::getBufferFromID (AudioBufferID bufferId)
{
    return buffers[bufferId];
}

AudioBufferManager::AudioBufferID AudioBufferManager::createBuffer ()
{
    AudioBufferID newBuffer (++ids, true);
    buffers[newBuffer] = std::make_shared<AudioBuffer<float>> (blockSize, newBuffer.getID ());
    ++numberBuffers;
    return newBuffer;
}

const AudioBufferManager::AudioBufferID AudioBufferManager::AudioBufferID::Empty = 
    AudioBufferManager::AudioBufferID (-1, false);
