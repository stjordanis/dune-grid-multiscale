#ifndef DUNE_GRID_MULTISCALE_PROVIDER_CUBE_HH
#define DUNE_GRID_MULTISCALE_PROVIDER_CUBE_HH

//#ifdef HAVE_DUNE_STUFF

// system
#include <vector>
#include <sstream>

// dune-common
#include <dune/common/exceptions.hh>
#include <dune/common/parametertree.hh>
#include <dune/common/shared_ptr.hh>

// dune-grid
#include <dune/grid/common/mcmgmapper.hh>
#include <dune/grid/io/file/vtk/vtkwriter.hh>

// dune-stuff
#include <dune/stuff/grid/provider/cube.hh>
#include <dune/stuff/common/logging.hh>

// dune-grid-multiscale
#include <dune/grid/multiscale/factory/default.hh>

namespace Dune {

namespace grid {

namespace Multiscale {

namespace Provider {

#ifdef HAVE_CONFIG_H
template< class GridImp = Dune::GridSelector::GridType >
#else
template< class GridImp >
#endif
class Cube
  : public Dune::Stuff::Grid::Provider::Cube< GridImp >
{
public:
  typedef Dune::Stuff::Grid::Provider::Cube< GridImp > BaseType;

  typedef typename BaseType::GridType GridType;

  typedef Cube< GridType > ThisType;

  static const std::string id;

  static const unsigned int dim = BaseType::dim;

  typedef typename BaseType::CoordinateType CoordinateType;

  typedef Dune::grid::Multiscale::Factory::Default< GridType > MsGridFactoryType;

  typedef typename MsGridFactoryType::MsGridType MsGridType;

  typedef typename MsGridType::GlobalGridPartType GlobalGridPartType;

  Cube(Dune::ParameterTree paramTree = Dune::ParameterTree())
    : BaseType(paramTree)
  {
    buildMsGrid(paramTree);
  }

  const MsGridType& msGrid() const
  {
    return *msGrid_;
  }

  const Dune::shared_ptr< const MsGridType > msGridPtr() const
  {
    return msGrid_;
  }

private:
  void buildMsGrid(const Dune::ParameterTree& paramTree)
  {
    // prepare
    const CoordinateType& lowerLeft = BaseType::lowerLeft();
    const CoordinateType& upperRight = BaseType::upperRight();
    MsGridFactoryType factory(BaseType::gridPtr());
    factory.prepare();

    // debug output
    const std::string prefix = "";
    Dune::Stuff::Common::LogStream& debug = Dune::Stuff::Common::Logger().debug();
    debug << prefix << id << ":" << std::endl;
    debug << prefix << "  lowerLeft: " << lowerLeft << std::endl;
    debug << prefix << "  upperRight: " << upperRight << std::endl;
    debug << prefix << "  partitions per dim: ";

    // get number of subdomains per direction
    std::vector< unsigned int > partitions(dim, 0);
    unsigned int totalSubdomains = 1;
    for (unsigned int d = 0; d < dim; ++d) {
      std::stringstream key;
      key << "partitions." << d;
      partitions[d] = paramTree.get(key.str(), 1);
      totalSubdomains *= partitions[d];
      debug << partitions[d] << " ";
    }
    debug << std::endl;

    // global grid part
    typedef typename MsGridType::GlobalGridPartType GridPartType;
    const Dune::shared_ptr< const GridPartType> gridPart = factory.globalGridPart();

    // walk the grid
    for (typename GridPartType::template Codim< 0 >::IteratorType it = gridPart->template begin< 0 >();
        it != gridPart->template end< 0 >();
        ++it) {
      // get center of entity
      typename GridType::LeafGridView::template Codim< 0 >::Iterator::Entity& entity = *it;
      const CoordinateType center = entity.geometry().global(entity.geometry().center());
//      debug << prefix << "  entity (" << center << "):" << std::endl;

      // decide on the subdomain this entity shall belong to
      std::vector< unsigned int > whichPartition;
      for (unsigned int d = 0; d < dim; ++d) {
        whichPartition.push_back(std::floor(partitions[d]*((center[d] - lowerLeft[d])/(upperRight[d] - lowerLeft[d]))));
      }
      unsigned int subdomain = 0;
      if (dim == 1)
        subdomain = whichPartition[0];
      else if (dim == 2)
        subdomain = whichPartition[0] + whichPartition[1]*partitions[0];
      else if (dim == 3)
        subdomain = whichPartition[0] + whichPartition[1]*partitions[0] + whichPartition[2]*partitions[1]*partitions[0];
      else {
        std::stringstream msg;
        msg << "Error in " << id << ": not implemented for grid dimensions other than 1, 2 or 3!";
        DUNE_THROW(Dune::NotImplemented, msg.str());
      } // decide on the subdomain this entity shall belong to

      // add entity to subdomain
      factory.add(entity, subdomain, prefix + "  ", debug);
    } // walk the grid

    // finalize
    factory.finalize(prefix + "  ", debug);
    debug << std::flush;

    msGrid_ = factory.createMsGrid();
  } // void buildMsGrid(const Dune::ParameterTree& paramTree)

  Dune::shared_ptr< const MsGridType > msGrid_;
}; // class Cube

template< class GridType >
const std::string Cube< GridType >::id = "grid.multiscale.provider.cube";

} // namespace Provider

} // namespace Multiscale

} // namespace Grid

} // namespace Dune

//#endif // HAVE_DUNE_STUFF

#endif // DUNE_GRID_MULTISCALE_PROVIDER_CUBE_HH
