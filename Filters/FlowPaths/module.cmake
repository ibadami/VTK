vtk_module(vtkFiltersFlowPaths
  GROUPS
    StandAlone
  DEPENDS
    vtkCommonExecutionModel
    vtkFiltersAMR
    vtkFiltersGeneral
    vtkFiltersSources
  TEST_DEPENDS
    vtkTestingCore
    vtkTestingRendering
    vtkRenderingOpenGL
  )