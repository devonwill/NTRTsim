/*
 * Copyright © 2012, United States Government, as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All rights reserved.
 * 
 * The NASA Tensegrity Robotics Toolkit (NTRT) v1 platform is licensed
 * under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0.
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied. See the License for the specific language
 * governing permissions and limitations under the License.
*/

/**
 * @file tgCompressionSpringActuator.cpp
 * @brief Contains the definitions of members of class tgCompressionSpringActuator.
 * @author Drew Sabelhaus, Brian Mirletz
 * @copyright Copyright (C) 2016 NASA Ames Research Center
 * $Id$
 */

// This Module
#include "tgBulletCompressionSpring.h"
#include "tgCompressionSpringActuator.h"
#include "tgModelVisitor.h"
#include "tgWorld.h"
// The Bullet Physics Library
#include "LinearMath/btQuickprof.h"

// The C++ Standard Library
#include <cmath>
#include <deque> // For history
#include <iostream>
#include <stdexcept>

using namespace std;

// Declaration of the config struct (...again? Hmmm...), copied from tgSpringCableActuator.
tgCompressionSpringActuator::Config::Config(double s,
                   double d,
                   double rL,
                   double rot) :
  stiffness(s),
  damping(d),
  restLength(rL),
  rotation(rot)        
{
    ///@todo is this the right place for this, or the constructor of this class?
    if (s < 0.0)
    {
        throw std::invalid_argument("stiffness is negative.");
    }
    else if (d < 0.0)
    {
        throw std::invalid_argument("damping is negative.");
    }
    else if (rL < 0.0)
    {
        throw std::invalid_argument("Rest length is negative. Compression springs have a positive rest length.");
    }
    else if (abs(rot) > M_PI * 2.0)
    {
	throw std::invalid_argument("Abs of rotation is greater than 2pi. Are you sure you're setting the right parameters?");
    }
}

// A helper function for the constructor.
// @TO-DO: Where is the "correct" place for these non-negative checks?
void tgCompressionSpringActuator::constructorAux()
{
    if (m_compressionSpring == NULL)
    {
        throw std::invalid_argument("Pointer to tgBulletCompressionSpring is NULL.");
    }
    else if (m_config.stiffness < 0.0)
    {
        throw std::invalid_argument("Stiffness is less than zero.");
    }
    else if (m_config.damping < 0.0)
    {
        throw std::invalid_argument("Damping coefficient is negative.");
    }
    else if (m_config.restLength < 0.0)
    {
        throw std::invalid_argument("Rest length is less than zero..");
    }
}

/**
 * The main constructor for this class.
 */
tgCompressionSpringActuator::tgCompressionSpringActuator(tgBulletCompressionSpring* compressionSpring,
                   const tgTags& tags,
                   tgCompressionSpringActuator::Config& config) :
    tgModel(tags),
    m_compressionSpring(compressionSpring),
    m_config(config),
    m_prevVelocity(0.0)
{
    // call the helper function that does some checks for non-negativeness
    constructorAux();

    // Postcondition
    assert(invariant());
    // making sure that no changes to the aux constructor function end up changing around
    // the compression spring pointer.
    assert(m_compressionSpring == compressionSpring);
}

tgCompressionSpringActuator::~tgCompressionSpringActuator()
{
    std::cout << "deleting compression spring within tgCompressionSpringActuator" << std::endl;
    delete m_compressionSpring;
}
    
void tgCompressionSpringActuator::setup(tgWorld& world)
{
    // This needs to be called here in case the controller needs to cast
    notifySetup();
    tgModel::setup(world);
}

void tgCompressionSpringActuator::teardown()
{
    // Do not notify teardown. Copied from tgBasicActuator/tgSpringCableActuator.
    tgModel::teardown();
}

/**
 * The step function calls on the tgBulletCompressionSpring to apply forces.
 * Since this class does not currently do any actuation, this function is the
 * only place that things get updated at each timestep.
 */
void tgCompressionSpringActuator::step(double dt) 
{
#ifndef BT_NO_PROFILE 
    BT_PROFILE("tgCompressionSpringActuator::step");
#endif //BT_NO_PROFILE   	
    if (dt <= 0.0)
    {
        throw std::invalid_argument("dt is not positive.");
    }
    else
    {   
        // Want to update any controls before applying forces
        notifyStep(dt); 
        m_compressionSpring->step(dt);
        tgModel::step(dt);
    }
}

// Renders the spring in the NTRT window
void tgBasicActuator::onVisit(const tgModelVisitor& r) const
{
#ifndef BT_NO_PROFILE 
    BT_PROFILE("tgCompressionSpringActuator::onVisit");
#endif //BT_NO_PROFILE	
    r.render(*this);
}
    
bool tgBasicActuator::invariant() const
{
    return
      (m_springCable != NULL);
}
