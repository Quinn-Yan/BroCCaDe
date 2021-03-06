/*******************************************************************************\
* Copyright (c) 2017 Murdoch University                                         *
*                                                                               *
* Written by Hendra Gunadi (Hendra.Gunadi@murdoch.edu.au)                       *
*                                                                               *
* Redistribution and use in source and binary forms, with or without            *
* modification, are permitted provided that the following conditions are met:   *
*                                                                               *
* (1) Redistributions of source code must retain the above copyright            *
*     notice, this list of conditions and the following disclaimer.             *
*                                                                               *
* (2) Redistributions in binary form must reproduce the above copyright         *
*     notice, this list of conditions and the following disclaimer in           *
*     the documentation and/or other materials provided with the                *
*     distribution.                                                             *
*                                                                               *
* (3) Neither the name of Hendra Gunadi and/or Murdoch University, nor          *
*     the names of contributors may be used to endorse or promote               *
*     products derived from this software without specific prior written        *
*     permission.                                                               *
*                                                                               *
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"   *
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE     *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE    *
* ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE     *
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR           *
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF          *
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS      *
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN       *
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)       *
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE    *
* POSSIBILITY OF SUCH DAMAGE.                                                   *
*                                                                               *
* Flow.cc : Implements Flow.h                                                   *
\*******************************************************************************/

#include <vector>			            // vector
#include <memory>			            // unique_ptr and shared_ptr
#include "Histogram.h"			        // Histogram
#include "Data_Container.h"		        // abstract class for data container
#include "Pattern_Data_Container.h"	    // CCE Pattern_Data
#include "Regularity_Data_Container.h"	// Regularity Regularity_Data
#include "KS_Flow.h"			        // KS test
#include "Entropy_Flow.h"		        // Entropy
#include "CCE_Flow.h"			        // CCE
#include "MultiModal_Flow.h"		    // MultiModality
#include "Autocorrelation_Flow.h"	    // Autocorrelation
#include "Regularity_Flow.h"		    // Regularity
#include "Flow.h"
#include <iostream>

//#define DEBUG_H

#ifdef DEBUG_H
#include <stdio.h>
#endif

// assume : idx < size
// check the bound for the set_IDs, tags, and analysis_id.
// if the vector size is not enough, then resize with the largest known possible
// values for the set_IDs, tags, and analysis_id
#define BOUND_CHECK_RESIZE(v, idx1, idx2, idx3, size1, size2, size3) \
	if (v.size() <= idx1) v.resize(size1); \
	if (v[idx1].size() <= idx2) v[idx1].resize(size2); \
	if (v[idx1][idx2].size() <= idx3) v[idx1][idx2].resize(size3);

using namespace CCD;

// Set up for adding data and analysis
void Flow::begin_adding_feature(unsigned int set_ID)
{
	_current_set_ID = set_ID;
	_result.clear();
    if (_steps.size() <= set_ID) _steps.resize(set_ID + 1, 0);
	_steps[set_ID]++;
}

void Flow::add_feature(unsigned int tag, std::vector<unsigned int> aid, double feature)
{
	// sanity check: the current set ID must be set, i.e. begin_adding_feature has been called
	if (_current_set_ID == -1) return;

	// flag to indicate whether the data insertion is duplicate
	std::vector<std::vector<bool>> flag(_config->tag_count, std::vector<bool>(_config->type_count, false));

	for (std::vector<unsigned int>::iterator it = aid.begin(); it != aid.end(); ++it)
	{
		Data_Container_Enum type = _config->map_analysis_data[*it];

		if (!flag[tag][type]) // if the data is not a duplicate
		{
			// it is assumed that the config has the highest possible values
			BOUND_CHECK_RESIZE(_data, (unsigned int) _current_set_ID, tag, type,
				_config->set_IDs, _config->tag_count, _config->type_count);

			if (_data[_current_set_ID][tag][type].get() == NULL)
			{
				// construct new data container
				switch (type)
				{
					case RAW_DATA :
					{
						_data[_current_set_ID][tag][type] = std::shared_ptr<Raw_Data>
							(new Raw_Data (_config->KS_window_size[_current_set_ID], 
							_config->step_sizes[_current_set_ID]));
						break;	
					}
					case HISTOGRAM_DATA :
					{
						_data[_current_set_ID][tag][type] = std::shared_ptr<Histogram>
							(new Histogram (_config->binner[*it][tag]));
						break;	
					}
					case PATTERN_DATA :
					{
						_data[_current_set_ID][tag][type] = 
							std::shared_ptr<Pattern_Data> (new Pattern_Data (
							_config->CCE_pattern_size[_current_set_ID], _config->binner[*it][tag]));
						break;
					}
					case REGULARITY_DATA :
					{
						_data[_current_set_ID][tag][type] = 
							std::shared_ptr<Regularity_Data> (new Regularity_Data (
								_config->Regularity_window_number[_current_set_ID],
								_config->Regularity_window_size[_current_set_ID]));
						break;
					}
					case NULL_DATA :
					{
						_data[_current_set_ID][tag][type] = _config->null_data;
						break;
					}
				}
			}
			_data[_current_set_ID][tag][type]->add_feature(feature);
			flag[tag][type] = true;
		}

		// if the calculation is trigerred, then do the analysis based on the current data set
		if (_steps[_current_set_ID] >= _config->step_sizes[_current_set_ID]) 
		{
            // actually trim the data now to reflect the correct window size
			if (type == RAW_DATA)
                std::static_pointer_cast<Raw_Data> (_data[_current_set_ID][tag][type])->reset_window();
			add_analysis(tag, *it);
		}
	}
}

void Flow::add_analysis(unsigned int tag, unsigned int aid)
{
	// it is assumed that the config has the highest possible values
	BOUND_CHECK_RESIZE(_analysis, (unsigned int) _current_set_ID, tag, aid, 
		_config->set_IDs, _config->tag_count, _config->number_of_analysis);

	if (_analysis[_current_set_ID][tag][aid].get() == NULL)
	{
		Data_Container_Enum type = _config->map_analysis_data[aid];

		// otherwise create a new analysis object and attach it to the flow
		// note: we lost the convenience to the analysis_ID enum since we can't
		// access the bif layer anymore. Currently we resorted to the if-else construct.
		if (aid == _config->KS_analysis)
			_analysis[_current_set_ID][tag][aid] = std::shared_ptr<KS> (new KS 
				(std::static_pointer_cast<Raw_Data>
				(_data[_current_set_ID][tag][type]), _config->KS_normal_data[tag]));			
		else if (aid == _config->Entropy_analysis)
			_analysis[_current_set_ID][tag][aid] = std::shared_ptr<Entropy>
				(new Entropy(std::static_pointer_cast<Histogram>
				(_data[_current_set_ID][tag][type])));	
		else if (aid == _config->CCE_analysis)
			_analysis[_current_set_ID][tag][aid] = std::shared_ptr<CCE>
				(new CCE(std::static_pointer_cast<Pattern_Data>
				(_data[_current_set_ID][tag][type])));
		else if (aid == _config->MultiModal_analysis) {
			_analysis[_current_set_ID][tag][aid] = std::shared_ptr<MultiModal>
				(new MultiModal(std::static_pointer_cast<Histogram>
				(_data[_current_set_ID][tag][type]))); }
		else if (aid == _config->Autocorrelation_analysis)
			_analysis[_current_set_ID][tag][aid] = std::shared_ptr<Autocorrelation>
				(new Autocorrelation(std::static_pointer_cast<Raw_Data>
				(_data[_current_set_ID][tag][type]), _config->Autocorrelation_lags[_current_set_ID]));
		else if (aid == _config->Regularity_analysis)
			_analysis[_current_set_ID][tag][aid] = std::shared_ptr<Regularity>
				(new Regularity(std::static_pointer_cast<Regularity_Data>
				(_data [_current_set_ID][tag][type])));
		else if (aid == _config->Null_analysis)
			_analysis[_current_set_ID][tag][aid] = _config->null_analysis;
	}

	// store the result and the identifier (aid and tag)
	_result.push_back(std::unique_ptr<TempValue>(new TempValue(
		_analysis[_current_set_ID][tag][aid]->calculate_metric(), aid, tag)));
}

bool Flow::end_adding_feature()
{
	bool calculation_trigger = false;
	// check if the calculation is triggered
	if (_steps[_current_set_ID] >= _config->step_sizes[_current_set_ID]) 
	{
		_steps[_current_set_ID] = 0;
		calculation_trigger = true;
	}
	_current_set_ID = -1;
	return calculation_trigger;
}

std::vector<std::unique_ptr<TempValue>> Flow::get_result()
{
	std::vector<std::unique_ptr<TempValue> > result;
	// We have no more use for the result, so just swap the whole vector.
	// This way we don't have to waste space to store the result internally
	result.swap(_result);
	return result;
}
