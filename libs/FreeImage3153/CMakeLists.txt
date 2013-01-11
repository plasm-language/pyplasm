
# see http://code.google.com/p/gamekit/source/browse/trunk/Dependencies/Source/FreeImage/CMakeLists.txt?r=647


set(FREEIMAGE_INCLUDE_DIRECTORIES
	Source/
	Source/DeprecationManager
	Source/FreeImage
	Source/LibJPEG
	Source/LibOpenJPEG
	Source/LibPNG
	Source/LibTIFF4
	Source/Metadata
	Source/ZLib
	Source/OpenEXR
	Source/OpenEXR/Half
	Source/OpenEXR/Iex
	Source/OpenEXR/IlmImf
	Source/OpenEXR/IlmThread
	Source/OpenEXR/Imath
	Source/LibRawLite
	)

add_definitions(-DOPJ_STATIC -DLIBRAW_NODLL  -DLIBRAW_LIBRARY_BUILD -DFREEIMAGE_LIB -DNO_LCMS)
include_directories(${FREEIMAGE_INCLUDE_DIRECTORIES})

# ---------------------------------------------------------
set(fi_ZLIB_SRCS 
	Source/ZLib/adler32.c
	Source/ZLib/compress.c
	Source/ZLib/crc32.c
	Source/ZLib/deflate.c
	Source/ZLib/infback.c
	Source/ZLib/inffast.c
	Source/ZLib/inflate.c
	Source/ZLib/inftrees.c
	Source/ZLib/trees.c
	Source/ZLib/uncompr.c
	Source/ZLib/zutil.c
)

# ---------------------------------------------------------
# FreeImage
set(fi_SRCS
	Source/FreeImage/BitmapAccess.cpp
	Source/FreeImage/CacheFile.cpp
	Source/FreeImage/ColorLookup.cpp
	Source/FreeImage/Conversion.cpp
	Source/FreeImage/Conversion16_555.cpp
	Source/FreeImage/Conversion16_565.cpp
	Source/FreeImage/Conversion24.cpp
	Source/FreeImage/Conversion32.cpp
	Source/FreeImage/Conversion4.cpp
	Source/FreeImage/Conversion8.cpp
	Source/FreeImage/ConversionRGBF.cpp
	Source/FreeImage/ConversionType.cpp
	Source/FreeImage/ConversionRGB16.cpp
	Source/FreeImage/FreeImage.cpp
	Source/FreeImage/FreeImageIO.cpp
	Source/FreeImage/GetType.cpp
	Source/FreeImage/Halftoning.cpp
	Source/FreeImage/J2KHelper.cpp
  Source/FreeImage/MNGHelper.cpp
	Source/FreeImage/MemoryIO.cpp
	Source/FreeImage/MultiPage.cpp
	Source/FreeImage/NNQuantizer.cpp
	Source/FreeImage/PSDParser.cpp
	Source/FreeImage/PixelAccess.cpp
	Source/FreeImage/Plugin.cpp
	Source/FreeImage/PluginBMP.cpp
	Source/FreeImage/PluginCUT.cpp
	Source/FreeImage/PluginDDS.cpp
	Source/FreeImage/PluginEXR.cpp
	Source/FreeImage/PluginG3.cpp
	Source/FreeImage/PluginGIF.cpp
	Source/FreeImage/PluginHDR.cpp
	Source/FreeImage/PluginICO.cpp
	Source/FreeImage/PluginIFF.cpp
	Source/FreeImage/PluginJ2K.cpp
	Source/FreeImage/PluginJNG.cpp
	Source/FreeImage/PluginJP2.cpp
	Source/FreeImage/PluginJPEG.cpp
	Source/FreeImage/PluginKOALA.cpp
	Source/FreeImage/PluginMNG.cpp
	Source/FreeImage/PluginPCD.cpp
	Source/FreeImage/PluginPCX.cpp
	Source/FreeImage/PluginPFM.cpp
	Source/FreeImage/PluginPICT.cpp
	Source/FreeImage/PluginPNG.cpp
	Source/FreeImage/PluginPNM.cpp
	Source/FreeImage/PluginPSD.cpp
	Source/FreeImage/PluginRAS.cpp
	Source/FreeImage/PluginRAW.cpp
	Source/FreeImage/PluginSGI.cpp
	Source/FreeImage/PluginTARGA.cpp
	Source/FreeImage/PluginTIFF.cpp
	Source/FreeImage/PluginWBMP.cpp
	Source/FreeImage/PluginXBM.cpp
	Source/FreeImage/PluginXPM.cpp
	Source/FreeImage/TIFFLogLuv.cpp
	Source/FreeImage/ToneMapping.cpp
	Source/FreeImage/WuQuantizer.cpp
	Source/FreeImage/ZLibInterface.cpp
	Source/FreeImage/tmoColorConvert.cpp
	Source/FreeImage/tmoDrago03.cpp
	Source/FreeImage/tmoFattal02.cpp
	Source/FreeImage/tmoReinhard05.cpp
)

# ---------------------------------------------------------
# FreeImageToolKit
set(fi_tk_SRCS
	Source/FreeImageToolkit/BSplineRotate.cpp
	Source/FreeImageToolkit/Background.cpp
	Source/FreeImageToolkit/Channels.cpp
	Source/FreeImageToolkit/ClassicRotate.cpp
	Source/FreeImageToolkit/Colors.cpp
	Source/FreeImageToolkit/CopyPaste.cpp
	Source/FreeImageToolkit/Display.cpp
	Source/FreeImageToolkit/Flip.cpp
	Source/FreeImageToolkit/JPEGTransform.cpp
	Source/FreeImageToolkit/MultigridPoissonSolver.cpp
	Source/FreeImageToolkit/Rescale.cpp
	Source/FreeImageToolkit/Resize.cpp
)


# ---------------------------------------------------------
# LibJPEG SubDir
set(fi_LibJPEG_SRCS
	#Source/LibJPEG/cdjpeg.c
	Source/LibJPEG/jaricom.c
	Source/LibJPEG/jcapimin.c
	Source/LibJPEG/jcapistd.c
	Source/LibJPEG/jcarith.c
	Source/LibJPEG/jccoefct.c
	Source/LibJPEG/jccolor.c
	Source/LibJPEG/jcdctmgr.c
	Source/LibJPEG/jchuff.c
	Source/LibJPEG/jcinit.c
	Source/LibJPEG/jcmainct.c
	Source/LibJPEG/jcmarker.c
	Source/LibJPEG/jcmaster.c
	Source/LibJPEG/jcomapi.c
	Source/LibJPEG/jcparam.c
	#Source/LibJPEG/jcphuff.c
	Source/LibJPEG/jcprepct.c
	Source/LibJPEG/jcsample.c
	Source/LibJPEG/jctrans.c
	Source/LibJPEG/jdapimin.c
	Source/LibJPEG/jdapistd.c
	Source/LibJPEG/jdarith.c
	Source/LibJPEG/jdatadst.c
	Source/LibJPEG/jdatasrc.c
	Source/LibJPEG/jdcoefct.c
	Source/LibJPEG/jdcolor.c
	Source/LibJPEG/jddctmgr.c
	Source/LibJPEG/jdhuff.c
	Source/LibJPEG/jdinput.c
	Source/LibJPEG/jdmainct.c
	Source/LibJPEG/jdmarker.c
	Source/LibJPEG/jdmaster.c
	Source/LibJPEG/jdmerge.c
	#Source/LibJPEG/jdphuff.c
	Source/LibJPEG/jdpostct.c
	Source/LibJPEG/jdsample.c
	Source/LibJPEG/jdtrans.c
	Source/LibJPEG/jerror.c
	Source/LibJPEG/jfdctflt.c
	Source/LibJPEG/jfdctfst.c
	Source/LibJPEG/jfdctint.c
	Source/LibJPEG/jidctflt.c
	Source/LibJPEG/jidctfst.c
	Source/LibJPEG/jidctint.c
	#Source/LibJPEG/jidctred.c
	Source/LibJPEG/jmemansi.c
	#Source/LibJPEG/jmemdos.c
	Source/LibJPEG/jmemmgr.c
	#Source/LibJPEG/jmemname.c
	#Source/LibJPEG/jmemnobs.c
	Source/LibJPEG/jquant1.c
	Source/LibJPEG/jquant2.c
	Source/LibJPEG/jutils.c
	Source/LibJPEG/rdbmp.c
	Source/LibJPEG/rdcolmap.c
	Source/LibJPEG/rdgif.c
	Source/LibJPEG/rdppm.c
	Source/LibJPEG/rdrle.c
	Source/LibJPEG/rdswitch.c
	Source/LibJPEG/rdtarga.c
	Source/LibJPEG/transupp.c
	Source/LibJPEG/wrbmp.c
	Source/LibJPEG/wrgif.c
	Source/LibJPEG/wrppm.c
	Source/LibJPEG/wrrle.c
	Source/LibJPEG/wrtarga.c
)

# ---------------------------------------------------------
# LibOpenJPEG SubDir
set(fi_LibOpenJPEG_SRCS	
	Source/LibOpenJPEG/bio.c
	Source/LibOpenJPEG/cio.c
	Source/LibOpenJPEG/dwt.c
	Source/LibOpenJPEG/event.c
	Source/LibOpenJPEG/image.c
	Source/LibOpenJPEG/j2k.c
	Source/LibOpenJPEG/j2k_lib.c
	Source/LibOpenJPEG/jp2.c
	Source/LibOpenJPEG/jpt.c
	Source/LibOpenJPEG/mct.c
	Source/LibOpenJPEG/mqc.c
	Source/LibOpenJPEG/openjpeg.c
	Source/LibOpenJPEG/pi.c
	Source/LibOpenJPEG/raw.c
	Source/LibOpenJPEG/t1.c
	Source/LibOpenJPEG/t2.c
	Source/LibOpenJPEG/tcd.c
	Source/LibOpenJPEG/tgt.c
	Source/LibOpenJPEG/cidx_manager.c
	Source/LibOpenJPEG/phix_manager.c
	Source/LibOpenJPEG/ppix_manager.c
	Source/LibOpenJPEG/thix_manager.c
	Source/LibOpenJPEG/tpix_manager.c
)

# ---------------------------------------------------------
# LibPNG SubDir
set(fi_LibPNG_SRCS
	Source/LibPNG/png.c
	Source/LibPNG/pngerror.c
	#Source/LibPNG/pnggccrd.c
	Source/LibPNG/pngget.c
	Source/LibPNG/pngmem.c
	Source/LibPNG/pngpread.c
	Source/LibPNG/pngread.c
	Source/LibPNG/pngrio.c
	Source/LibPNG/pngrtran.c
	Source/LibPNG/pngrutil.c
	Source/LibPNG/pngset.c
	Source/LibPNG/pngtrans.c
	#Source/LibPNG/pngvcrd.c
	Source/LibPNG/pngwio.c
	Source/LibPNG/pngwrite.c
	Source/LibPNG/pngwtran.c
	Source/LibPNG/pngwutil.c
)


# ---------------------------------------------------------
# LibTIFF4 SubDir
set(fi_LibTIFF4_SRCS	
	Source/LibTIFF4/tif_aux.c
	Source/LibTIFF4/tif_close.c
	Source/LibTIFF4/tif_codec.c
	Source/LibTIFF4/tif_color.c
	Source/LibTIFF4/tif_compress.c
	Source/LibTIFF4/tif_dir.c
	Source/LibTIFF4/tif_dirinfo.c
	Source/LibTIFF4/tif_dirread.c
	Source/LibTIFF4/tif_dirwrite.c
	Source/LibTIFF4/tif_dumpmode.c
	Source/LibTIFF4/tif_error.c
	Source/LibTIFF4/tif_extension.c
	Source/LibTIFF4/tif_fax3.c
	Source/LibTIFF4/tif_fax3sm.c
	Source/LibTIFF4/tif_flush.c
	Source/LibTIFF4/tif_getimage.c
	Source/LibTIFF4/tif_jbig.c
	Source/LibTIFF4/tif_jpeg.c
	Source/LibTIFF4/tif_jpeg_12.c
	Source/LibTIFF4/tif_luv.c
	Source/LibTIFF4/tif_lzma.c
	Source/LibTIFF4/tif_lzw.c
	Source/LibTIFF4/tif_next.c
	Source/LibTIFF4/tif_ojpeg.c
	Source/LibTIFF4/tif_open.c
	Source/LibTIFF4/tif_packbits.c
	Source/LibTIFF4/tif_pixarlog.c
	Source/LibTIFF4/tif_predict.c
	Source/LibTIFF4/tif_print.c
	Source/LibTIFF4/tif_read.c
	Source/LibTIFF4/tif_strip.c
	Source/LibTIFF4/tif_swab.c
	Source/LibTIFF4/tif_thunder.c
	Source/LibTIFF4/tif_tile.c
	Source/LibTIFF4/tif_version.c
	Source/LibTIFF4/tif_warning.c
	Source/LibTIFF4/tif_write.c
	Source/LibTIFF4/tif_zip.c)


# ---------------------------------------------------------
# Metadata
set(fi_Metadata_SRCS
	Source/Metadata/Exif.cpp
	Source/Metadata/FIRational.cpp
	Source/Metadata/FreeImageTag.cpp
	Source/Metadata/IPTC.cpp
	Source/Metadata/TagConversion.cpp
	Source/Metadata/TagLib.cpp
	Source/Metadata/XTIFF.cpp
)


# ---------------------------------------------------------
# LibRawLite 
set(fi_rawlite_SRCS 
	Source/LibRawLite/src/libraw_cxx.cpp
	Source/LibRawLite/src/libraw_c_api.cpp
	Source/LibRawLite/src/libraw_datastream.cpp
	Source/LibRawLite/internal/dcraw_common.cpp
	Source/LibRawLite/internal/dcraw_fileio.cpp
	Source/LibRawLite/internal/demosaic_packs.cpp
  #Source/LibRawLite/internal/dcb_demosaicing.c
	)

# ---------------------------------------------------------
# OpenEXR SubDir

set(fi_OpenEXR_SRCS
	Source/OpenEXR/Half/half.cpp
	Source/OpenEXR/Iex/IexBaseExc.cpp
	Source/OpenEXR/Iex/IexThrowErrnoExc.cpp
	Source/OpenEXR/IlmImf/ImfAttribute.cpp
	Source/OpenEXR/IlmImf/ImfB44Compressor.cpp
	Source/OpenEXR/IlmImf/ImfBoxAttribute.cpp
	Source/OpenEXR/IlmImf/ImfCRgbaFile.cpp
	Source/OpenEXR/IlmImf/ImfChannelList.cpp
	Source/OpenEXR/IlmImf/ImfChannelListAttribute.cpp
	Source/OpenEXR/IlmImf/ImfChromaticities.cpp
	Source/OpenEXR/IlmImf/ImfChromaticitiesAttribute.cpp
	Source/OpenEXR/IlmImf/ImfCompressionAttribute.cpp
	Source/OpenEXR/IlmImf/ImfCompressor.cpp
	Source/OpenEXR/IlmImf/ImfConvert.cpp
	Source/OpenEXR/IlmImf/ImfDoubleAttribute.cpp
	Source/OpenEXR/IlmImf/ImfEnvmap.cpp
	Source/OpenEXR/IlmImf/ImfEnvmapAttribute.cpp
	Source/OpenEXR/IlmImf/ImfFloatAttribute.cpp
	Source/OpenEXR/IlmImf/ImfFrameBuffer.cpp
	Source/OpenEXR/IlmImf/ImfFramesPerSecond.cpp
	Source/OpenEXR/IlmImf/ImfHeader.cpp
	Source/OpenEXR/IlmImf/ImfHuf.cpp
	Source/OpenEXR/IlmImf/ImfIO.cpp
	Source/OpenEXR/IlmImf/ImfInputFile.cpp
	Source/OpenEXR/IlmImf/ImfIntAttribute.cpp
	Source/OpenEXR/IlmImf/ImfKeyCode.cpp
	Source/OpenEXR/IlmImf/ImfKeyCodeAttribute.cpp
	Source/OpenEXR/IlmImf/ImfLineOrderAttribute.cpp
	Source/OpenEXR/IlmImf/ImfLut.cpp
	Source/OpenEXR/IlmImf/ImfMatrixAttribute.cpp
	Source/OpenEXR/IlmImf/ImfMisc.cpp
	Source/OpenEXR/IlmImf/ImfMultiView.cpp	
	Source/OpenEXR/IlmImf/ImfOpaqueAttribute.cpp
	Source/OpenEXR/IlmImf/ImfOutputFile.cpp
	Source/OpenEXR/IlmImf/ImfPizCompressor.cpp
	Source/OpenEXR/IlmImf/ImfPreviewImage.cpp
	Source/OpenEXR/IlmImf/ImfPreviewImageAttribute.cpp
	Source/OpenEXR/IlmImf/ImfPxr24Compressor.cpp
	Source/OpenEXR/IlmImf/ImfRational.cpp
	Source/OpenEXR/IlmImf/ImfRationalAttribute.cpp
	Source/OpenEXR/IlmImf/ImfRgbaFile.cpp
	Source/OpenEXR/IlmImf/ImfRgbaYca.cpp
	Source/OpenEXR/IlmImf/ImfRleCompressor.cpp
	Source/OpenEXR/IlmImf/ImfScanLineInputFile.cpp
	Source/OpenEXR/IlmImf/ImfStandardAttributes.cpp
	Source/OpenEXR/IlmImf/ImfStdIO.cpp
	Source/OpenEXR/IlmImf/ImfStringAttribute.cpp
	Source/OpenEXR/IlmImf/ImfStringVectorAttribute.cpp	
	Source/OpenEXR/IlmImf/ImfTestFile.cpp
	Source/OpenEXR/IlmImf/ImfThreading.cpp
	Source/OpenEXR/IlmImf/ImfTileDescriptionAttribute.cpp
	Source/OpenEXR/IlmImf/ImfTileOffsets.cpp
	Source/OpenEXR/IlmImf/ImfTiledInputFile.cpp
	Source/OpenEXR/IlmImf/ImfTiledMisc.cpp
	Source/OpenEXR/IlmImf/ImfTiledOutputFile.cpp
	Source/OpenEXR/IlmImf/ImfTiledRgbaFile.cpp
	Source/OpenEXR/IlmImf/ImfTimeCode.cpp
	Source/OpenEXR/IlmImf/ImfTimeCodeAttribute.cpp
	Source/OpenEXR/IlmImf/ImfVecAttribute.cpp
	Source/OpenEXR/IlmImf/ImfVersion.cpp
	Source/OpenEXR/IlmImf/ImfWav.cpp
	Source/OpenEXR/IlmImf/ImfZipCompressor.cpp
	#Source/OpenEXR/IlmImf/b44ExpLogTable.cpp
	Source/OpenEXR/IlmThread/IlmThreadPool.cpp
	Source/OpenEXR/Imath/ImathBox.cpp
	Source/OpenEXR/Imath/ImathColorAlgo.cpp
	Source/OpenEXR/Imath/ImathFun.cpp
	Source/OpenEXR/Imath/ImathMatrixAlgo.cpp
	Source/OpenEXR/Imath/ImathRandom.cpp
	Source/OpenEXR/Imath/ImathShear.cpp
	Source/OpenEXR/Imath/ImathVec.cpp	
		)

if (WIN32)
	set(fi_OpenEXR_SRCS_PLAT
			Source/OpenEXR/IlmThread/IlmThreadMutexWin32.cpp
			Source/OpenEXR/IlmThread/IlmThreadSemaphoreWin32.cpp
			Source/OpenEXR/IlmThread/IlmThreadWin32.cpp
			)
else (WIN32)			   
	set(fi_OpenEXR_SRCS_PLAT
			Source/OpenEXR/IlmThread/IlmThreadSemaphore.cpp
			Source/OpenEXR/IlmThread/IlmThread.cpp
			Source/OpenEXR/IlmThread/IlmThreadMutex.cpp
			Source/OpenEXR/IlmThread/IlmThreadPosix.cpp
			Source/OpenEXR/IlmThread/IlmThreadMutexPosix.cpp
			Source/OpenEXR/IlmThread/IlmThreadSemaphorePosix.cpp
			Source/OpenEXR/IlmThread/IlmThreadSemaphorePosixCompat.cpp
			)
endif (WIN32)


# ---------------------------------------------------------
set(FREEIMAGE_ALLSOURCE
	${fi_SRCS}
	${fi_tk_SRCS}
	${fi_LibJPEG_SRCS}
	${fi_LibMNG_SRCS}
	${fi_LibOpenJPEG_SRCS}
	${fi_LibPNG_SRCS}
	${fi_LibTIFF4_PLAT}
	${fi_LibTIFF4_SRCS}
	${fi_Metadata_SRCS}
	${fi_OpenEXR_SRCS}
	${fi_OpenEXR_SRCS_PLAT}
	${fi_rawlite_SRCS}
	${fi_ZLIB_SRCS}
	)

add_library(FreeImage STATIC ${FREEIMAGE_ALLSOURCE})



