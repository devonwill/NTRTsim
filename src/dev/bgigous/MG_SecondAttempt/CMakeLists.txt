link_libraries(learningSpines
               boost_program_options
               tgcreator             
               core
               util
               terrain
               Adapters
               Configuration
               AnnealEvolution
               tgOpenGLSupport
               obstacles
               sensors
               controllers)

add_executable(AppQuadSimpleActuation
               AppQuadSimpleActuation.cpp
               JSONQuadFeedbackControl.cpp
               BigPuppySymmetricSpiral2.cpp)
				

target_link_libraries(AppQuadSimpleActuation ${ENV_LIB_DIR}/libjsoncpp.a FileHelpers boost_program_options obstacles JSONControl)
