// This class provides a way of generating a spectrum given user-defined
// parameters. A spectrum is generated by summing up a series of Gaussian
// distributions defined on the spectrum curve. Each Gaussian represents a peak
// in the spectral, which typically correlates with a specific element or
// material type.

#ifndef SRC_SPECTRUM_SPECTRUM_GENERATOR_H_
#define SRC_SPECTRUM_SPECTRUM_GENERATOR_H_

#include <vector>

namespace hsi_data_generator {
namespace spectrum_generator {

// The distribution variables for a single spectrum peak. These values encode a
// Gaussian distribution that represents a single peak in the spectrum. All
// values should be normalized between 0 and 1.
struct PeakDistribution {
  // The position is the x-position of the peak. The peak will be centered at
  // this position. It is the mean of the Guassian.
  //
  // Position should be normalized between 0 and 1.
  double peak_position;

  // The amplitude is the "height" of the peak. 0 is nothing, 1 is the maximum
  // height.
  double amplitude;

  // The width is the standard deviation of the Gaussian, and encodes how thin
  // or thick the peak will be. Sharp, instantaneous peaks should have a small
  // width.
  //
  // A peak's width should be smaller than the span of the entire spectrum,
  // which is a normalized width of 1.0.
  double width;
};

// Generates the spectrum given a set of PeakDistributions. This is done by
// sampling each band from the summed up mixture of Guassians in the given
// list.
//
// The returned spectrum is automatically scaled between 0 and 1. It is
// possible that the maximum value is less than 1.
std::vector<double> GenerateSpectrum(
    const std::vector<PeakDistribution>& peaks, const int num_bands);

}  // namespace spectrum_generator
}  // namespace hsi_data_generator

#endif  // SRC_SPECTRUM_SPECTRUM_GENERATOR_H_