/**
 *  \file     filter.c
 *  \brief    1st and 2nd order filter implementation
 *  \author   Jean Baptiste Lanfrey <jean-baptiste.lanfrey@parrot.com>
 *  \version  1.0
 */

#define _XOPEN_SOURCE 600
#include <Maths/maths.h>
#include <Maths/filter.h>
#include <math.h>

void filter_init(uint32_t n, float32_t *old_input, float32_t initial_input, float32_t *old_output, float32_t initial_output)
{
  uint32_t ii;

  for (ii=0; ii<n ; ii++)
    old_input[ii]  = initial_input;

  for (ii=0; ii<n; ii++)
    old_output[ii] = initial_output;
}

float filter(uint32_t n, const float32_t *b, const float32_t *a, float32_t input, float32_t *old_input, float32_t *old_output)
{
  uint32_t ii;
  float32_t inno, past, output;

  // innovation computation
  inno = b[0] * input;
  for (ii=1; ii<n+1; ii++)
    inno += b[ii] * old_input[ii-1];

  // past computation
  past = 0.0;
  for (ii=1; ii<n+1; ii++)
    past -= a[ii] * old_output[ii-1];

  // output = (inno + past) / a[0];
	// We assume a[0] is always equal to 1
  output = (inno + past);

  // inputs and outputs shift
  for (ii=n-1; ii>0; ii--)
    old_input[ii] = old_input[ii-1];

  old_input[0] = input;

  for (ii=n-1; ii>0; ii--)
    old_output[ii] = old_output[ii-1];

  old_output[0] = output;

  return output;
}


// Filtre du type Kd.p / (1+Td.p)
float32_t deriv(deriv_param_t *param, float32_t input)
{
  static float32_t exp_1 = 0.3678794411714423412f; // expf(-1.0f);

  float32_t exp;
  float32_t td;
  float32_t out;

  if( f_is_zero( param->td ) )
  {
    // prevent from dividing by 0.
    td  = param->te;
    exp = exp_1;
  }
  else
  {
    td  = param->td;
    // exp = expf(-param->te/td);
    exp = exp_taylor(-param->te/td);
  }

  param->internal_state = exp * param->internal_state + td * (1 - exp) * param->old_input;

  out = ( param->kd / td ) * ( input - param->internal_state / td );

  param->old_input = input;

  return out;
}

void delay_init(uint32_t m, float32_t *old_input, float32_t initial_input)
{
  uint32_t ii;

  for (ii=0; ii<m ; ii++)
    old_input[ii] = initial_input;
}

float32_t delay(uint32_t m, float32_t input, float32_t *old_input)
{
  uint32_t ii;
  float32_t output;

  output = old_input[m-1];

  // inputs and shift
  for (ii=m-1; ii>0; ii--)
    old_input[ii] = old_input[ii-1];

  old_input[0] = input;

  return output;
}

float32_t rate_limiter(float32_t input, float32_t old_output, float32_t rate_max)
{
  float32_t output;
  float_or_int_t FOI_rate,FOI_rate_max;

  FOI_rate.f = input - old_output;
  FOI_rate_max.f = rate_max;

  if( f_abs( FOI_rate ) > FOI_rate_max.f )
    output = old_output + f_set_clamp( FOI_rate, FOI_rate_max );
  else
    output = input;

  return output;
}

int32_t digitalsmooth(int32_t rawIn, int32_t *sensSmoothArray)
{
  int32_t j, k, temp, top, bottom;
  long total;
  static int32_t i;

  static int32_t sorted[filterSamples];
  bool_t done;

  i = (i + 1) % filterSamples;
  sensSmoothArray[i] = rawIn;


  for (j=0; j<filterSamples; j++)
  {
    sorted[j] = sensSmoothArray[j];
  }

  done = 0;
  while(done != 1)
  {
    done = 1;
    for (j = 0; j < (filterSamples - 1); j++)
      {
      if (sorted[j] > sorted[j + 1])
        {
        temp = sorted[j + 1];
        sorted [j+1] =  sorted[j] ;
        sorted [j] = temp;
        done = 0;
        }
      }
  }

  bottom = max_macro(((filterSamples * 15)  / 100), 1);
  top = min_macro((((filterSamples * 85) / 100) + 1  ), (filterSamples - 1));
  k = 0;
  total = 0;
  for ( j = bottom; j< top; j++){
    total += sorted[j];
    k++;
  }

  return total / k;
}


//unwrapToPi
// takes pointer to the value of wrapped angle, pointer to the value of unwrapped angle, and the variation to add to both angles
void unwrapToPi(float32_t* former_wrapped_angle, float32_t* former_unwrapped_angle, float32_t variation)
{
    if(fabsf(variation)<PI)
    {
        *former_unwrapped_angle += variation;
    }

    else
    {
        if(variation>=0)
        {
            *former_unwrapped_angle += variation - 2.0*PI;
        }
        else
        {
            *former_unwrapped_angle += variation + 2.0*PI;
        }
    }

    wrapToPi(former_unwrapped_angle, former_wrapped_angle);
}

void wrapToPi(float32_t* input, float32_t* output)
{
	*output = fmodf(*input + PI, 2*PI) - PI;
    if((-PI)>(*input))
    {
        *output += 2*PI;
    }
}


void filter64_init(uint32_t n, float64_t *old_input, float64_t initial_input, float64_t *old_output, float64_t initial_output)
{
  uint32_t ii;

  for (ii=0; ii<n ; ii++)
    old_input[ii]  = initial_input;

  for (ii=0; ii<n; ii++)
    old_output[ii] = initial_output;
}

float64_t filter64(uint32_t n, const float64_t *b, const float64_t *a, float64_t input, float64_t *old_input, float64_t *old_output)
{
  uint32_t ii;
  float64_t inno, past, output;

  // innovation computation
  inno = b[0] * input;
  for (ii=1; ii<n+1; ii++)
    inno += b[ii] * old_input[ii-1];

  // past computation
  past = 0.0;
  for (ii=1; ii<n+1; ii++)
    past -= a[ii] * old_output[ii-1];

  // output = (inno + past) / a[0];
	// We assume a[0] is always equal to 1
  output = (inno + past);

  // inputs and outputs shift
  for (ii=n-1; ii>0; ii--)
    old_input[ii] = old_input[ii-1];

  old_input[0] = input;

  for (ii=n-1; ii>0; ii--)
    old_output[ii] = old_output[ii-1];

  old_output[0] = output;

  return output;
}
