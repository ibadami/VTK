/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkPUnstructuredGridGhostCellsGenerator.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// .NAME vtkPUnstructuredGridGhostCellsGenerator - Builds ghost cells for a
//  distributed unstructured grid dataset.
//
// .SECTION Description
// This filter generate ghost cells for distributed a unstructured grid in
// parallel - using MPI asynchronous communications.
// The filter can take benefit of the input grid point global ids to perform.
//
// .SECTION Caveats
//  <ul>
//    <li> A step of 'all reduce' (each processor send/receive data to/from
//         all other processors.
//    <li> The code currently assumes one grid per rank. </li>
//    <li> PointData and CellData must match across partitions/processes. </li>
//  </ul>
//
// .SECTION See Also
// vtkDistributedDataFilter vtkPUnstructuredGridGhostDataGenerator
//
// .SECTION Thanks
// This filter has been developed by Joachim Pouderoux, Kitware SAS 2015.
//
// This filter was expanded to compute multiple ghost layers by Boonthanome
// Nouanesengsy and John Patchett, Los Alamos National Laboratory 2016.
//
// ************************************************
//
// This filter uses different algorithms when obtaining the first layer of
// ghost cells and getting subsequent layers.
//
// First ghost cell layer algorithm:
//   - each proc obtains surface points using the surface filter
//   - perform an all-to-all to share surface points with each other
//   - for each other proc, look at their points, and see if any points
//     match any of your local points
//   - for each matching point, find all local cells which use those points,
//     and send those cells to that proc. mark the cells that were sent
//     (used for later ghost layers)
//   - receive all cells sent to you, and merge everything together
//
// Subsequent ghost layers
//   - for each cell that was sent last round, find all other local cells
//     which border these cells. 'local cells' also includes all ghost cells
//     which i have. send these cells to the same proc, and mark them as sent
//     last round
//   - receive all cells sent to you, and merge everything together
//   - if another layer is needed, repeat
//

#ifndef vtkPUnstructuredGridGhostCellsGenerator_h
#define vtkPUnstructuredGridGhostCellsGenerator_h

#include "vtkFiltersParallelGeometryModule.h" // For export macro
#include "vtkUnstructuredGridAlgorithm.h"

class vtkMultiProcessController;
class vtkUnstructuredGrid;
class vtkUnstructuredGridBase;

class VTKFILTERSPARALLELGEOMETRY_EXPORT vtkPUnstructuredGridGhostCellsGenerator:
  public vtkUnstructuredGridAlgorithm
{
  vtkTypeMacro(vtkPUnstructuredGridGhostCellsGenerator, vtkUnstructuredGridAlgorithm);

public:
  void PrintSelf(ostream& os, vtkIndent indent);

  static vtkPUnstructuredGridGhostCellsGenerator *New();

  // Description:
  // Set/Get the MPI multi process controller object.
  void SetController(vtkMultiProcessController *c);
  vtkGetObjectMacro(Controller, vtkMultiProcessController);

  // Description:
  // Specify if the filter must take benefit of global point ids if they exist.
  // If false, point coordinates are used. Default is TRUE.
  vtkSetMacro(UseGlobalPointIds, bool);
  vtkGetMacro(UseGlobalPointIds, bool);
  vtkBooleanMacro(UseGlobalPointIds, bool);

  // Description:
  // Specify the name of the global point ids data array if the GlobalIds
  // attribute array is not set. Default is "GlobalNodeIds".
  vtkSetStringMacro(GlobalPointIdsArrayName);
  vtkGetStringMacro(GlobalPointIdsArrayName);

  // Description:
  // Specify if the data has global cell ids.
  // If more than one layer of ghost cells is needed, global cell ids are
  // necessary. If global cell ids are not provided, they will be computed
  // internally.
  // If false, global cell ids will be computed, then deleted afterwards.
  // Default is FALSE.
  vtkSetMacro(HasGlobalCellIds, bool);
  vtkGetMacro(HasGlobalCellIds, bool);
  vtkBooleanMacro(HasGlobalCellIds, bool);

  // Description:
  // Specify the name of the global cell ids data array if the GlobalIds
  // attribute array is not set. Default is "GlobalNodeIds".
  vtkSetStringMacro(GlobalCellIdsArrayName);
  vtkGetStringMacro(GlobalCellIdsArrayName);

  // Description:
  // Specify if the filter must generate the ghost cells only if required by
  // the pipeline.
  // If false, ghost cells are computed even if they are not required.
  // Default is TRUE.
  vtkSetMacro(BuildIfRequired, bool);
  vtkGetMacro(BuildIfRequired, bool);
  vtkBooleanMacro(BuildIfRequired, bool);

protected:
  vtkPUnstructuredGridGhostCellsGenerator();
  ~vtkPUnstructuredGridGhostCellsGenerator();

  virtual int RequestData(vtkInformation *, vtkInformationVector **,
    vtkInformationVector *);

  void GetFirstGhostLayer(int, vtkUnstructuredGrid *);

  void ExtractAndReduceSurfacePoints();

  void ComputeSharedPoints();

  void ExtractAndSendGhostCells(vtkUnstructuredGridBase *);

  void ReceiveAndMergeGhostCells(int, vtkUnstructuredGridBase *,
    vtkUnstructuredGrid *);

  void AddGhostLayer(int ghostLevel, int maxGhostLevel);

  void FindGhostCells();

  void AddGlobalCellIds();

  void RemoveGlobalCellIds();


  vtkMultiProcessController *Controller;

  int NumRanks;
  int RankId;
  char *GlobalPointIdsArrayName;
  bool UseGlobalPointIds;
  char *GlobalCellIdsArrayName;
  bool HasGlobalCellIds;
  bool BuildIfRequired;

private:
  struct vtkInternals;
  vtkInternals *Internals;

  vtkPUnstructuredGridGhostCellsGenerator(const vtkPUnstructuredGridGhostCellsGenerator&); // Not implemented
  void operator=(const vtkPUnstructuredGridGhostCellsGenerator&); // Not implemented
};

#endif
