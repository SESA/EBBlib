/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* (C) Copyright IBM Corp.  2007, 2007                              */
/* IBM CPL License                                                  */
/*                                                                  */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
/**
 * \file common/bgp_UCI.h
 */

#ifndef _BGP_UCI_H_  // Prevent multiple inclusion.
#define _BGP_UCI_H_



//
// The Packaging WorkBook, as defined in "Naming_Convention_BGP_060209.pdf"
//  has a naming convention for every component in a BlueGene machine.
//
// The following structures allow representation of all components in 32bits.
// Currently, 17 different components/connectors/modules have been assigned a
//  precise naming convention that uniquely identifies that component within
//  a Blue Gene installation. This convention can represent a machine of up to
//  256 Racks, and can uniquely identify everything right down to an individual
//  DDR SRAAM Module on a particular node.
//
// This information is used by Node System Software in 2 ways:
//   1) Precise identification of failing components when reporting RAS events.
//   2) Locating the node within the machine to understand the layout of
//       the Block (or Partition) for functions such as Route calculation.
//

#include <common/namespace.h>

__BEGIN_DECLS

#ifdef _AIX
#include <inttypes.h>
#else
#ifdef __KERNEL__
/* We will not keep this forever */
#include <linux/types.h>
#else
#include <stdint.h>
#endif
#endif

#include <common/bgp_bitnumbers.h>


#define _BGP_UCI_Component_Rack              ( 0)
#define _BGP_UCI_Component_Midplane          ( 1)
#define _BGP_UCI_Component_BulkPowerSupply   ( 2)
#define _BGP_UCI_Component_PowerCable        ( 3)
#define _BGP_UCI_Component_PowerModule       ( 4)
#define _BGP_UCI_Component_ClockCard         ( 5)
#define _BGP_UCI_Component_FanAssembly       ( 6)
#define _BGP_UCI_Component_Fan               ( 7)
#define _BGP_UCI_Component_ServiceCard       ( 8)
#define _BGP_UCI_Component_LinkCard          ( 9)
#define _BGP_UCI_Component_LinkChip          (10)
#define _BGP_UCI_Component_LinkPort          (11)  // Identifies 1 end of a LinkCable
#define _BGP_UCI_Component_NodeCard          (12)
#define _BGP_UCI_Component_ComputeCard       (13)
#define _BGP_UCI_Component_IOCard            (14)
#define _BGP_UCI_Component_DDRChip           (15)
#define _BGP_UCI_Component_ENetConnector     (16)

typedef struct _BGP_UCI_Rack_t
                {                           // "Rxy": R<RackRow><RackColumn>
                unsigned Component   :  5;  // when _BGP_UCI_Component_Rack
                unsigned RackRow     :  4;  // 0..F
                unsigned RackColumn  :  4;  // 0..F
                unsigned _zero       : 19;  // zero's
                }
                _BGP_UCI_Rack_t;

#define _BGP_UCI_RACK_COMPONENT(x)              _B5( 4,x)  // when _BGP_UCI_Component_Rack
#define _BGP_UCI_RACK_RACKROW(x)                _B4( 8,x)  // 0..F
#define _BGP_UCI_RACK_RACKCOLUMN(x)             _B4(12,x)  // 0..F



typedef struct _BGP_UCI_Midplane_t
                {                           // "Rxy-Mm": R<RackRow><RackColumn>-M<Midplane>
                unsigned Component   :  5;  // when _BGP_UCI_Component_Midplane
                unsigned RackRow     :  4;  // 0..F
                unsigned RackColumn  :  4;  // 0..F
                unsigned Midplane    :  1;  // 0=Bottom, 1=Top
                unsigned _zero       : 18;  // zero's
                }
                _BGP_UCI_Midplane_t;

#define _BGP_UCI_MIDPLANE_COMPONENT(x)          _B5( 4,x)  // when _BGP_UCI_Component_Midplane
#define _BGP_UCI_MIDPLANE_RACKROW(x)            _B4( 8,x)  // 0..F
#define _BGP_UCI_MIDPLANE_RACKCOLUMN(x)         _B4(12,x)  // 0..F
#define _BGP_UCI_MIDPLANE_MIDPLANE(x)           _B1(13,x)  // 0=Bottom, 1=Top



typedef struct _BGP_UCI_BulkPowerSupply_t
                {                           // "Rxy-B": R<RackRow><RackColumn>-B
                unsigned Component   :  5;  // when _BGP_UCI_Component_BulkPowerSupply
                unsigned RackRow     :  4;  // 0..F
                unsigned RackColumn  :  4;  // 0..F
                unsigned _zero       : 19;  // zero's
                }
                _BGP_UCI_BulkPowerSupply_t;

#define _BGP_UCI_BULKPOWERSUPPLY_COMPONENT(x)   _B5( 4,x)  // when _BGP_UCI_Component_BulkPowerSupply
#define _BGP_UCI_BULKPOWERSUPPLY_RACKROW(x)     _B4( 8,x)  // 0..F
#define _BGP_UCI_BULKPOWERSUPPLY_RACKCOLUMN(x)  _B4(12,x)  // 0..F



typedef struct _BGP_UCI_PowerCable_t
                {                           // "Rxy-B-C": R<RackRow><RackColumn>-B-C
                unsigned Component   :  5;  // when _BGP_UCI_Component_PowerCable
                unsigned RackRow     :  4;  // 0..F
                unsigned RackColumn  :  4;  // 0..F
                unsigned _zero       : 19;  // zero's
                }
                _BGP_UCI_PowerCable_t;

#define _BGP_UCI_POWERCABLE_COMPONENT(x)        _B5( 4,x)  // when _BGP_UCI_Component_PowerCable
#define _BGP_UCI_POWERCABLE_RACKROW(x)          _B4( 8,x)  // 0..F
#define _BGP_UCI_POWERCABLE_RACKCOLUMN(x)       _B4(12,x)  // 0..F



typedef struct _BGP_UCI_PowerModule_t
                {                           // "Rxy-B-Pp": R<RackRow><RackColumn>-B-P<PowerModule>
                unsigned Component   :  5;  // when _BGP_UCI_Component_PowerModule
                unsigned RackRow     :  4;  // 0..F
                unsigned RackColumn  :  4;  // 0..F
                unsigned PowerModule :  3;  // 0..7 (0..3 left to right facing front, 4-7 left to right facing rear)
                unsigned _zero       : 16;  // zero's
                }
                _BGP_UCI_PowerModule_t;

#define _BGP_UCI_POWERMODULE_COMPONENT(x)       _B5( 4,x)  // when _BGP_UCI_Component_PowerModule
#define _BGP_UCI_POWERMODULE_RACKROW(x)         _B4( 8,x)  // 0..F
#define _BGP_UCI_POWERMODULE_RACKCOLUMN(x)      _B4(12,x)  // 0..F
#define _BGP_UCI_POWERMODULE_POWERMODULE(x)     _B3(15,x)  // 0..7 (0..3 left to right facing front, 4-7 left to right facing rear)



typedef struct _BGP_UCI_ClockCard_t
                {                           // "Rxy-K": R<RackRow><RackColumn>-K
                unsigned Component   :  5;  // when _BGP_UCI_Component_ClockCard
                unsigned RackRow     :  4;  // 0..F
                unsigned RackColumn  :  4;  // 0..F
                unsigned _zero       : 19;  // zero's
                }
                _BGP_UCI_ClockCard_t;

#define _BGP_UCI_CLOCKCARD_COMPONENT(x)         _B5( 4,x)  // when _BGP_UCI_Component_PowerModule
#define _BGP_UCI_CLOCKCARD_RACKROW(x)           _B4( 8,x)  // 0..F
#define _BGP_UCI_CLOCKCARD_RACKCOLUMN(x)        _B4(12,x)  // 0..F



typedef struct _BGP_UCI_FanAssembly_t
                {                           // "Rxy-Mm-Aa": R<RackRow><RackColumn>-M<Midplane>-A<FanAssembly>
                unsigned Component   :  5;  // when _BGP_UCI_Component_FanAssembly
                unsigned RackRow     :  4;  // 0..F
                unsigned RackColumn  :  4;  // 0..F
                unsigned Midplane    :  1;  // 0=Bottom, 1=Top
                unsigned FanAssembly :  4;  // 0..9 (0=Bot Front, 4=Top Front, 5=Bot Rear, 9=Top Rear)
                unsigned _zero       : 14;  // zero's
                }
                _BGP_UCI_FanAssembly_t;

#define _BGP_UCI_FANASSEMBLY_COMPONENT(x)       _B5( 4,x)  // when _BGP_UCI_Component_FanAssembly
#define _BGP_UCI_FANASSEMBLY_RACKROW(x)         _B4( 8,x)  // 0..F
#define _BGP_UCI_FANASSEMBLY_RACKCOLUMN(x)      _B4(12,x)  // 0..F
#define _BGP_UCI_FANASSEMBLY_MIDPLANE(x)        _B1(13,x)  // 0=Bottom, 1=Top
#define _BGP_UCI_FANASSEMBLY_FANASSEMBLY(x)     _B4(17,x)  // 0..9 (0=Bot Front, 4=Top Front, 5=Bot Rear, 9=Top Rear)



typedef struct _BGP_UCI_Fan_t
                {                           // "Rxy-Mm-Aa-Ff": R<RackRow><RackColumn>-M<Midplane>-A<FanAssembly>-F<Fan>
                unsigned Component   :  5;  // when _BGP_UCI_Component_Fan
                unsigned RackRow     :  4;  // 0..F
                unsigned RackColumn  :  4;  // 0..F
                unsigned Midplane    :  1;  // 0=Bottom, 1=Top
                unsigned FanAssembly :  4;  // 0..9 (0=Bot Front, 4=Top Front, 5=Bot Rear, 9=Top Rear)
                unsigned Fan         :  2;  // 0..2 (0=Tailstock, 2=Midplane)
                unsigned _zero       : 12;  // zero's
                }
                _BGP_UCI_Fan_t;

#define _BGP_UCI_FAN_COMPONENT(x)               _B5( 4,x)  // when _BGP_UCI_Component_Fan
#define _BGP_UCI_FAN_RACKROW(x)                 _B4( 8,x)  // 0..F
#define _BGP_UCI_FAN_RACKCOLUMN(x)              _B4(12,x)  // 0..F
#define _BGP_UCI_FAN_MIDPLANE(x)                _B1(13,x)  // 0=Bottom, 1=Top
#define _BGP_UCI_FAN_FANASSEMBLY(x)             _B4(17,x)  // 0..9 (0=Bot Front, 4=Top Front, 5=Bot Rear, 9=Top Rear)
#define _BGP_UCI_FAN_FAN(x)                     _B2(19,x)  // 0..2 (0=Tailstock, 2=Midplane)



typedef struct _BGP_UCI_ServiceCard_t
                {                           // "Rxy-Mm-S": R<RackRow><RackColumn>-M<Midplane>-S
                unsigned Component   :  5;  // when _BGP_UCI_Component_ServiceCard
                unsigned RackRow     :  4;  // 0..F
                unsigned RackColumn  :  4;  // 0..F
                unsigned Midplane    :  1;  // 0=Bottom, 1=Top (Master ServiceCard in M0)
                unsigned _zero       : 18;  // zero's
                }
                _BGP_UCI_ServiceCard_t;

#define _BGP_UCI_SERVICECARD_COMPONENT(x)       _B5( 4,x)  // when _BGP_UCI_Component_ServiceCard
#define _BGP_UCI_SERVICECARD_RACKROW(x)         _B4( 8,x)  // 0..F
#define _BGP_UCI_SERVICECARD_RACKCOLUMN(x)      _B4(12,x)  // 0..F
#define _BGP_UCI_SERVICECARD_MIDPLANE(x)        _B1(13,x)  // 0=Bottom, 1=Top (Master ServiceCard in M0)



typedef struct _BGP_UCI_LinkCard_t
                {                           // "Rxy-Mm-Ll": R<RackRow><RackColumn>-M<Midplane>-L<LinkCard>
                unsigned Component   :  5;  // when _BGP_UCI_Component_LinkCard
                unsigned RackRow     :  4;  // 0..F
                unsigned RackColumn  :  4;  // 0..F
                unsigned Midplane    :  1;  // 0=Bottom, 1=Top
                unsigned LinkCard    :  2;  // 0..3: 0=BF, 1=TF, 2=BR, 3=TR)
                unsigned _zero       : 16;  // zero's
                }
                _BGP_UCI_LinkCard_t;

#define _BGP_UCI_LINKCARD_COMPONENT(x)          _B5( 4,x)  // when _BGP_UCI_Component_LinkCard
#define _BGP_UCI_LINKCARD_RACKROW(x)            _B4( 8,x)  // 0..F
#define _BGP_UCI_LINKCARD_RACKCOLUMN(x)         _B4(12,x)  // 0..F
#define _BGP_UCI_LINKCARD_MIDPLANE(x)           _B1(13,x)  // 0=Bottom, 1=Top
#define _BGP_UCI_LINKCARD_LINKCARD(x)           _B2(15,x)  // 0..3: 0=BF, 1=TF, 2=BR, 3=TR)



typedef struct _BGP_UCI_LinkChip_t
                {                           // "Rxy-Mm-Ll-Uu": R<RackRow><RackColumn>-M<Midplane>-L<LinkCard>-U<LinkChip>
                unsigned Component   :  5;  // when _BGP_UCI_Component_LinkChip
                unsigned RackRow     :  4;  // 0..F
                unsigned RackColumn  :  4;  // 0..F
                unsigned Midplane    :  1;  // 0=Bottom, 1=Top
                unsigned LinkCard    :  2;  // 0..3: 0=BF, 1=TF, 2=BR, 3=TR)
                unsigned LinkChip    :  3;  // 00..05: left to right from Front
                unsigned _zero       : 13;  // zero's
                }
                _BGP_UCI_LinkChip_t;

#define _BGP_UCI_LINKCHIP_COMPONENT(x)          _B5( 4,x)  // when _BGP_UCI_Component_LinkChip
#define _BGP_UCI_LINKCHIP_RACKROW(x)            _B4( 8,x)  // 0..F
#define _BGP_UCI_LINKCHIP_RACKCOLUMN(x)         _B4(12,x)  // 0..F
#define _BGP_UCI_LINKCHIP_MIDPLANE(x)           _B1(13,x)  // 0=Bottom, 1=Top
#define _BGP_UCI_LINKCHIP_LINKCARD(x)           _B2(15,x)  // 0..3: 0=BF, 1=TF, 2=BR, 3=TR)
#define _BGP_UCI_LINKCHIP_LINKCHIP(x)           _B3(18,x)  // 00..05: left to right from Front



typedef struct _BGP_UCI_LinkPort_t
                {                           // "Rxy-Mm-Ll-Jjj": R<RackRow><RackColumn>-M<Midplane>-L<LinkCard>-J<LinkPort>
                unsigned Component   :  5;  // when _BGP_UCI_Component_LinkPort
                unsigned RackRow     :  4;  // 0..F
                unsigned RackColumn  :  4;  // 0..F
                unsigned Midplane    :  1;  // 0=Bottom, 1=Top
                unsigned LinkCard    :  2;  // 0..3: 0=BF, 1=TF, 2=BR, 3=TR)
                unsigned LinkPort    :  4;  // 00..15: left to right from Front
                unsigned _zero       : 12;  // zero's
                }
                _BGP_UCI_LinkPort_t;

#define _BGP_UCI_LINKPORT_COMPONENT(x)          _B5( 4,x)  // when _BGP_UCI_Component_LinkPort
#define _BGP_UCI_LINKPORT_RACKROW(x)            _B4( 8,x)  // 0..F
#define _BGP_UCI_LINKPORT_RACKCOLUMN(x)         _B4(12,x)  // 0..F
#define _BGP_UCI_LINKPORT_MIDPLANE(x)           _B1(13,x)  // 0=Bottom, 1=Top
#define _BGP_UCI_LINKPORT_LINKCARD(x)           _B2(15,x)  // 0..3: 0=BF, 1=TF, 2=BR, 3=TR)
#define _BGP_UCI_LINKPORT_LINKPORT(x)           _B4(19,x)  // 00..15: left to right from Front



typedef struct _BGP_UCI_NodeCard_t
                {                           // "Rxy-Mm-Nnn": R<RackRow><RackColumn>-M<Midplane>-N<NodeCard>
                unsigned Component   :  5;  // when _BGP_UCI_Component_NodeCard
                unsigned RackRow     :  4;  // 0..F
                unsigned RackColumn  :  4;  // 0..F
                unsigned Midplane    :  1;  // 0=Bottom, 1=Top
                unsigned NodeCard    :  4;  // 00..15: 00=BF, 07=TF, 08=BR, 15=TR)
                unsigned _zero       : 14;  // zero's
                }
                _BGP_UCI_NodeCard_t;

#define _BGP_UCI_NODECARD_COMPONENT(x)          _B5( 4,x)  // when _BGP_UCI_Component_NodeCard
#define _BGP_UCI_NODECARD_RACKROW(x)            _B4( 8,x)  // 0..F
#define _BGP_UCI_NODECARD_RACKCOLUMN(x)         _B4(12,x)  // 0..F
#define _BGP_UCI_NODECARD_MIDPLANE(x)           _B1(13,x)  // 0=Bottom, 1=Top
#define _BGP_UCI_NODECARD_NODECARD(x)           _B4(17,x)  // 00..15: 00=BF, 07=TF, 08=BR, 15=TR)



typedef struct _BGP_UCI_ComputeCard_t
                {                           // "Rxy-Mm-Nnn-Jxx": R<RackRow><RackColumn>-M<Midplane>-N<NodeCard>-J<ComputeCard>
                unsigned Component   :  5;  // when _BGP_UCI_Component_ComputeCard
                unsigned RackRow     :  4;  // 0..F
                unsigned RackColumn  :  4;  // 0..F
                unsigned Midplane    :  1;  // 0=Bottom, 1=Top
                unsigned NodeCard    :  4;  // 00..15: 00=BF, 07=TF, 08=BR, 15=TR)
                unsigned ComputeCard :  6;  // 04..35 (00-01 IOCard, 02-03 Reserved, 04-35 ComputeCard)
                unsigned _zero       :  8;  // zero's
                }
                _BGP_UCI_ComputeCard_t;

#define _BGP_UCI_COMPUTECARD_COMPONENT(x)       _B5( 4,x)  // when _BGP_UCI_Component_ComputeCard
#define _BGP_UCI_COMPUTECARD_RACKROW(x)         _B4( 8,x)  // 0..F
#define _BGP_UCI_COMPUTECARD_RACKCOLUMN(x)      _B4(12,x)  // 0..F
#define _BGP_UCI_COMPUTECARD_MIDPLANE(x)        _B1(13,x)  // 0=Bottom, 1=Top
#define _BGP_UCI_COMPUTECARD_NODECARD(x)        _B4(17,x)  // 00..15: 00=BF, 07=TF, 08=BR, 15=TR)
#define _BGP_UCI_COMPUTECARD_COMPUTECARD(x)     _B6(23,x)  // 04..35 (00-01 IOCard, 02-03 Reserved, 04-35 ComputeCard)



typedef struct _BGP_UCI_IOCard_t
                {                           // "Rxy-Mm-Nnn-Jxx": R<RackRow><RackColumn>-M<Midplane>-N<NodeCard>-J<ComputeCard>
                unsigned Component   :  5;  // when _BGP_UCI_Component_IOCard
                unsigned RackRow     :  4;  // 0..F
                unsigned RackColumn  :  4;  // 0..F
                unsigned Midplane    :  1;  // 0=Bottom, 1=Top
                unsigned NodeCard    :  4;  // 00..15: 00=BF, 07=TF, 08=BR, 15=TR)
                unsigned ComputeCard :  6;  // 00..01 (00-01 IOCard, 02-03 Reserved, 04-35 ComputeCard)
                unsigned _zero       :  8;  // zero's
                }
                _BGP_UCI_IOCard_t;

#define _BGP_UCI_IOCARD_COMPONENT(x)            _B5( 4,x)  // when _BGP_UCI_Component_IOCard
#define _BGP_UCI_IOCARD_RACKROW(x)              _B4( 8,x)  // 0..F
#define _BGP_UCI_IOCARD_RACKCOLUMN(x)           _B4(12,x)  // 0..F
#define _BGP_UCI_IOCARD_MIDPLANE(x)             _B1(13,x)  // 0=Bottom, 1=Top
#define _BGP_UCI_IOCARD_NODECARD(x)             _B4(17,x)  // 00..15: 00=BF, 07=TF, 08=BR, 15=TR)
#define _BGP_UCI_IOCARD_COMPUTECARD(x)          _B6(23,x)  // 00..01 (00-01 IOCard, 02-03 Reserved, 04-35 ComputeCard)



typedef struct _BGP_UCI_DDRChip_t
                {                           // "Rxy-Mm-Nnn-Jxx-Uuu": R<RackRow><RackColumn>-M<Midplane>-N<NodeCard>-J<ComputeCard>-U<DDRChip>
                unsigned Component   :  5;  // when _BGP_UCI_Component_DDRChip
                unsigned RackRow     :  4;  // 0..F
                unsigned RackColumn  :  4;  // 0..F
                unsigned Midplane    :  1;  // 0=Bottom, 1=Top
                unsigned NodeCard    :  4;  // 00..15: 00=BF, 07=TF, 08=BR, 15=TR)
                unsigned ComputeCard :  6;  // 00..01 (00-01 IOCard, 02-03 Reserved, 04-35 ComputeCard)
                unsigned DDRChip     :  5;  // 00..20
                unsigned _zero       :  3;  // zero's
                }
                _BGP_UCI_DDRChip_t;

#define _BGP_UCI_DDRCHIP_COMPONENT(x)           _B5( 4,x)  // when _BGP_UCI_Component_DDRChip
#define _BGP_UCI_DDRCHIP_RACKROW(x)             _B4( 8,x)  // 0..F
#define _BGP_UCI_DDRCHIP_RACKCOLUMN(x)          _B4(12,x)  // 0..F
#define _BGP_UCI_DDRCHIP_MIDPLANE(x)            _B1(13,x)  // 0=Bottom, 1=Top
#define _BGP_UCI_DDRCHIP_NODECARD(x)            _B4(17,x)  // 00..15: 00=BF, 07=TF, 08=BR, 15=TR)
#define _BGP_UCI_DDRCHIP_COMPUTECARD(x)         _B6(23,x)  // 00..01 (00-01 IOCard, 02-03 Reserved, 04-35 ComputeCard)
#define _BGP_UCI_DDRCHIP_DDRCHIP(x)             _B5(28,x)  // 00..20



typedef struct _BGP_UCI_ENetConnector_t
                {                           // "Rxy-Mm-Nnn-ENe": R<RackRow><RackColumn>-M<Midplane>-N<NodeCard>-EN<EN>
                unsigned Component   :  5;  // when _BGP_UCI_Component_ENetConnector
                unsigned RackRow     :  4;  // 0..F
                unsigned RackColumn  :  4;  // 0..F
                unsigned Midplane    :  1;  // 0=Bottom, 1=Top
                unsigned NodeCard    :  4;  // 00..15: 00=BF, 07=TF, 08=BR, 15=TR)
                unsigned EN          :  1;  // 0..1 (Equal to IOCard number)
                unsigned _zero       : 13;  // zero's
                }
                _BGP_UCI_ENetConnector_t;

#define _BGP_UCI_ENETCONNECTOR_COMPONENT(x)     _B5( 4,x)  // when _BGP_UCI_Component_ENetConnector
#define _BGP_UCI_ENETCONNECTOR_RACKROW(x)       _B4( 8,x)  // 0..F
#define _BGP_UCI_ENETCONNECTOR_RACKCOLUMN(x)    _B4(12,x)  // 0..F
#define _BGP_UCI_ENETCONNECTOR_MIDPLANE(x)      _B1(13,x)  // 0=Bottom, 1=Top
#define _BGP_UCI_ENETCONNECTOR_NODECARD(x)      _B4(17,x)  // 00..15: 00=BF, 07=TF, 08=BR, 15=TR)
#define _BGP_UCI_ENETCONNECTOR_ENETCONNECTOR(x) _B1(18,x)  // 0..1 (Equal to IOCard number)



typedef union  T_BGP_UniversalComponentIdentifier
                {
                uint32_t                   UCI;
                _BGP_UCI_Rack_t            Rack;
                _BGP_UCI_Midplane_t        Midplane;
                _BGP_UCI_BulkPowerSupply_t BulkPowerSupply;
                _BGP_UCI_PowerCable_t      PowerCable;
                _BGP_UCI_PowerModule_t     PowerModule;
                _BGP_UCI_ClockCard_t       ClockCard;
                _BGP_UCI_FanAssembly_t     FanAssembly;
                _BGP_UCI_Fan_t             Fan;
                _BGP_UCI_ServiceCard_t     ServiceCard;
                _BGP_UCI_LinkCard_t        LinkCard;
                _BGP_UCI_LinkChip_t        LinkChip;
                _BGP_UCI_LinkPort_t        LinkPort;
                _BGP_UCI_NodeCard_t        NodeCard;
                _BGP_UCI_ComputeCard_t     ComputeCard;
                _BGP_UCI_IOCard_t          IOCard;
                _BGP_UCI_DDRChip_t         DDRChip;
                _BGP_UCI_ENetConnector_t   ENetConnector;
                }
                _BGP_UniversalComponentIdentifier;


extern inline uint32_t _bgp_Make_ComputeCard_UCI( unsigned rackrow,
                                                  unsigned rackcolumn,
                                                  unsigned midplane,
                                                  unsigned nodecard,
                                                  unsigned computecard )
{
   _BGP_UniversalComponentIdentifier uci;

   uci.UCI = (_BGP_UCI_COMPUTECARD_COMPONENT(_BGP_UCI_Component_ComputeCard) |
              _BGP_UCI_COMPUTECARD_RACKROW(rackrow)                          |
              _BGP_UCI_COMPUTECARD_RACKCOLUMN(rackcolumn)                    |
              _BGP_UCI_COMPUTECARD_MIDPLANE(midplane)                        |
              _BGP_UCI_COMPUTECARD_NODECARD(nodecard)                        |
              _BGP_UCI_COMPUTECARD_COMPUTECARD(computecard)                    );


   return( uci.UCI );
}

extern inline uint32_t _bgp_Make_IOCard_UCI( unsigned rackrow,
                                             unsigned rackcolumn,
                                             unsigned midplane,
                                             unsigned nodecard,
                                             unsigned computecard )
{
   _BGP_UniversalComponentIdentifier uci;

   uci.UCI = (_BGP_UCI_COMPUTECARD_COMPONENT(_BGP_UCI_Component_IOCard) |
              _BGP_UCI_COMPUTECARD_RACKROW(rackrow)                     |
              _BGP_UCI_COMPUTECARD_RACKCOLUMN(rackcolumn)               |
              _BGP_UCI_COMPUTECARD_MIDPLANE(midplane)                   |
              _BGP_UCI_COMPUTECARD_NODECARD(nodecard)                   |
              _BGP_UCI_COMPUTECARD_COMPUTECARD(computecard)              );


   return( uci.UCI );
}

//
// Back of Node Card (connection to MidPlane)
//
//  JC331  JC321   JC311  JC301
//  JC330  JC320   JC310  JC300
//  JC031  JC021   JC011  JC001
//  JC030  JC020   JC010  JC000
//  JC201  JC211   JC221  JC231
//  JC200  JC210   JC220  JC230
//  JC101  JC111   JC121  JC131
//  JC100  JC110   JC120  JC130
//  JI0    JI1     JI2    JI3
//
// Front of Node Card
//


__END_DECLS



#endif // Add nothing below this line.
