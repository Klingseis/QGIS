/***************************************************************************
    qgsmeshstaticdatasetwidget.cpp
    -------------------------------------
    begin                : March 2020
    copyright            : (C) 2020 by Vincent Cloarec
    email                : vcloarec at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgsmeshstaticdatasetwidget.h"

#include "qgsmeshlayer.h"

QgsMeshStaticDatasetWidget::QgsMeshStaticDatasetWidget( QWidget *parent ): QWidget( parent )
{
  setupUi( this );

  mDatasetScalarModel = new QgsMeshDatasetListModel( this );
  mScalarDatasetComboBox->setModel( mDatasetScalarModel );
  mDatasetVectorModel = new QgsMeshDatasetListModel( this );
  mVectorDatasetComboBox->setModel( mDatasetVectorModel );
}

void QgsMeshStaticDatasetWidget::setLayer( QgsMeshLayer *layer )
{
  mLayer = layer;
}

void QgsMeshStaticDatasetWidget::syncToLayer()
{
  if ( !mLayer || !mLayer->dataProvider() )
    return;

  mDatasetScalarModel->setMeshLayer( mLayer );
  mDatasetVectorModel->setMeshLayer( mLayer );
  setScalarDatasetGroup( mLayer->rendererSettings().activeScalarDatasetGroup() );
  setVectorDatasetGroup( mLayer->rendererSettings().activeVectorDatasetGroup() );
}

void QgsMeshStaticDatasetWidget::apply()
{
  if ( !mLayer )
    return;

  mLayer->setStaticScalarDatasetIndex( QgsMeshDatasetIndex( mScalarDatasetGroup, mScalarDatasetComboBox->currentIndex() - 1 ) );
  mLayer->setStaticVectorDatasetIndex( QgsMeshDatasetIndex( mVectorDatasetGroup, mVectorDatasetComboBox->currentIndex() - 1 ) );
}

void QgsMeshStaticDatasetWidget::setScalarDatasetGroup( int index )
{
  mScalarDatasetGroup = index;
  mDatasetScalarModel->setDatasetGroup( index );
  mScalarDatasetComboBox->setEnabled( mScalarDatasetGroup >= 0 );
  if ( mLayer && mLayer->dataProvider() )
  {
    mScalarName->setText( mLayer->dataProvider()->datasetGroupMetadata( index ).name() );
    setScalarDatasetIndex( mLayer->staticScalarDatasetIndex().dataset() );
  }
}

void QgsMeshStaticDatasetWidget::setVectorDatasetGroup( int index )
{
  mVectorDatasetGroup = index;
  mDatasetVectorModel->setDatasetGroup( index );
  mVectorDatasetComboBox->setEnabled( mVectorDatasetGroup >= 0 );
  if ( mLayer && mLayer->dataProvider() )
  {
    mVectorName->setText( mLayer->dataProvider()->datasetGroupMetadata( index ).name() );
    setVectorDatasetIndex( mLayer->staticVectorDatasetIndex().dataset() );
  }
}

void QgsMeshStaticDatasetWidget::setScalarDatasetIndex( int index )
{
  if ( index < mLayer->dataProvider()->datasetCount( mScalarDatasetGroup ) )
    mScalarDatasetComboBox->setCurrentIndex( index + 1 );
  else
    mScalarDatasetComboBox->setCurrentIndex( 0 );
}

void QgsMeshStaticDatasetWidget::setVectorDatasetIndex( int index )
{
  if ( index < mLayer->dataProvider()->datasetCount( mVectorDatasetGroup ) )
    mVectorDatasetComboBox->setCurrentIndex( index + 1 );
  else
    mVectorDatasetComboBox->setCurrentIndex( 0 );
}

QgsMeshDatasetListModel::QgsMeshDatasetListModel( QObject *parent ): QAbstractListModel( parent )
{}

void QgsMeshDatasetListModel::setMeshLayer( QgsMeshLayer *layer )
{
  beginResetModel();
  mLayer = layer;
  endResetModel();
}

void QgsMeshDatasetListModel::setDatasetGroup( int group )
{
  beginResetModel();
  mDatasetGroup = group;
  endResetModel();
}

int QgsMeshDatasetListModel::rowCount( const QModelIndex &parent ) const
{
  Q_UNUSED( parent )

  if ( mLayer && mLayer->dataProvider() )
    return  mLayer->dataProvider()->datasetCount( mDatasetGroup ) + 1;
  else
    return 0;
}

QVariant QgsMeshDatasetListModel::data( const QModelIndex &index, int role ) const
{
  if ( !index.isValid() )
    return QVariant();

  if ( role == Qt::DisplayRole )
  {
    if ( !mLayer || !mLayer->dataProvider() || mDatasetGroup < 0 || index.row() == 0 )
      return tr( "none" );

    else if ( index.row() == 1 && mLayer->dataProvider()->datasetCount( mDatasetGroup ) == 1 )
    {
      return tr( "Display dataset" );
    }
    else
    {
      qint64 time = mLayer->dataProvider()->temporalCapabilities()->datasetTime( QgsMeshDatasetIndex( mDatasetGroup, index.row() - 1 ) );
      return mLayer->formatTime( time / 3600.0 / 1000.0 );
    }
  }

  return QVariant();
}
