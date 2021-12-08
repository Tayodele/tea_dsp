#include "TeaFIR.h"
#include "TeaCommon/conversions.h"
#include "fftw3.h"

using namespace TTModules;

//FIR
TeaFIR::TeaFIR()
{
  init();
}

TeaFIR::TeaFIR(double afs, double ord)
{
  init(afs, ord);
}

void TeaFIR::init(double afs, double ord)
{
  fs = afs;
  cutoff = 0.;
  order = ord;
  coeffs = new double[order];
  buffer = new double[order];
  //using constant for now, want to test the speed of this
  resp_size = int(1 << 6);
  buildFilter();
}

TeaFIR::~TeaFIR()
{
  delete coeffs;
}

double TeaFIR::getCutoff()
{
  return cutoff;
}

void TeaFIR::setCutoff(double val)
{
  cutoff = val;
  buildFilter();
}

int TeaFIR::getOrder()
{
  return order;
}
void TeaFIR::setOrder(int val)
{
  delete coeffs;
  delete buffer;
  if (val < 2)
    order = 2;
  order = val * 2;
  coeffs = new double[order];
  buffer = new double[order];
  setCoeffs();
}

TeaFIR::filtertype TeaFIR::getType()
{
  return type;
}
void TeaFIR::setType(filtertype val)
{
  type = val;
  buildFilter();
}

double *TeaFIR::getResponse()
{
  return fresp;
}

int TeaFIR::getResponseSize()
{
  return resp_size;
}

void TeaFIR::buildFilter()
{
  // Rectangle Response
  auto resp_delta = (fs) / resp_size;
  double *rect_resp = new double[resp_size];
  double *fresp = new double[resp_size];
  auto kind = fftw_r2r_kind(1);
  unsigned flags = 0;
  fftw_plan plan;
  plan = fftw_plan_r2r_1d(resp_size, rect_resp, fresp, kind, flags);
  auto mirror_len = -1;
  for (int i = 0; i < resp_size; i++)
  {
    rect_resp[i] = 0;
    if (i * resp_delta <= cutoff)
      rect_resp[i] = 1;
    else
    {
      if (mirror_len < 0)
        mirror_len = i;
      if (i >= resp_size - mirror_len)
      {
        rect_resp[i] = 1;
        mirror_len--;
      }
    }
  }
  // IFFT
  fftw_execute(plan);
  // Window

  setCoeffs();
  fftw_destroy_plan(plan);
}

// Passes one sample block thorugh the filter
// Maybe put the convolution as a general function?
void TeaFIR::filter(double &input, double &output)
{
  for (auto i = order - 1; i > 0; i--)
  {
    buffer[i] = buffer[i - 1];
  }
  buffer[0] = input;
  auto temp = 0.;
  for (auto j = 0; j < order; j++)
  {
    temp += buffer[j] * coeffs[order - j - 1];
  }
  output = temp;
}

void TeaFIR::setCoeffs()
{
  int half = resp_size / 2 - order / 2;
  for (int i = 0; i < order; i++)
  {
    coeffs[i] = fresp[half + i];
  }
}