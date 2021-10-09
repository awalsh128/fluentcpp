message(STATUS "Copying artwork to directories.")

set(WEBSITE_IMAGES_DIR ${PROJECT_SOURCE_DIR}/assets/website/images)
file(COPY 
  ${CMAKE_CURRENT_SOURCE_DIR}/fluentcpp-sm.png
  ${CMAKE_CURRENT_SOURCE_DIR}/fluentcpp-initial-sm.png  
  DESTINATION ${WEBSITE_IMAGES_DIR})