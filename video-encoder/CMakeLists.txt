cmake_minimum_required(VERSION 2.8)
project( video-encoder )

MESSAGE ("\n==> Finding 3rd Party Packages")
find_package( OpenCV REQUIRED )

SET( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -W" )
SET( CMAKE_C_FLAGS_RELEASE "-O3" )
SET( CMAKE_C_FLAGS_DEBUG "-g3" )

SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall" )
SET( CMAKE_CXX_FLAGS_RELEASE "-O3" )
SET( CMAKE_CXX_FLAGS_DEBUG "-g3" )

SET( BASE_DIR ${CMAKE_SOURCE_DIR} )
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BASE_DIR}/bin )

# Make include directories available for all..
INCLUDE_DIRECTORIES( 
	${OpenCV_INCLUDE_DIR}
)

add_executable( SpatialEncoder spatial_encoder.cpp BitStream.cpp Golomb.cpp Predictor.cpp )
target_link_libraries( SpatialEncoder ${OpenCV_LIBS} )

add_executable( SpatialDecoder spatial_decoder.cpp BitStream.cpp Golomb.cpp Predictor.cpp )
target_link_libraries( SpatialDecoder ${OpenCV_LIBS} )

add_executable( LoadVideo LoadRawVideo.cpp )
target_link_libraries( LoadVideo ${OpenCV_LIBS} )

add_executable( TemporalEncoder temporal_encoder.cpp BitStream.cpp Golomb.cpp Predictor.cpp )
target_link_libraries( TemporalEncoder ${OpenCV_LIBS} ) 

add_executable( TemporalDecoder temporal_decoder.cpp BitStream.cpp Golomb.cpp Predictor.cpp )
target_link_libraries( TemporalDecoder ${OpenCV_LIBS} ) 

add_executable( HybEncoder hybrid_encoder.cpp BitStream.cpp Golomb.cpp Predictor.cpp )
target_link_libraries( HybEncoder ${OpenCV_LIBS} ) 

add_executable( HybDecoder hybrid_decoder.cpp BitStream.cpp Golomb.cpp Predictor.cpp )
target_link_libraries( HybDecoder ${OpenCV_LIBS} ) 
