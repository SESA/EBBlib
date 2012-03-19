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
 * \file common/bgp_personality_inlines.h
 */

#ifndef _BGP_PERSONALITY_INLINES_H_ // Prevent multiple inclusion
#define _BGP_PERSONALITY_INLINES_H_

/**********************************************************************
 *
 *	$Id: bgp_personality_inlines.h,v 1.25 2007/10/18 18:01:52 mtn Exp $
 *
 *	Project:  BlueGene/P
 *
 *	Creation: 2006/08/08
 *
 *	$Author: mtn $
 *	$Source: /BGP/CVS/bgp/arch/include/common/bgp_personality_inlines.h,v $
 *	$Revision: 1.25 $
 *	$State: Exp $
 *	$Date: 2007/10/18 18:01:52 $
 *	$Locker:  $
 *
 **********************************************************************/

#include <common/bgp_personality.h>
#ifdef __KERNEL__
/* Assume "kernel" means Linux */
#include <linux/types.h>
#include <asm/byteorder.h>
#include <endian.h>
#else
#include <stdio.h>
#include <stdint.h>
#include <endian.h>
#endif

/* The personality is stored internally as big-endian.  Always.
 * Use the inline accessor functions below or be VERY careful.
 */
#if __BYTE_ORDER == __LITTLE_ENDIAN
#define fixEndian16(x) \
        ((uint16_t)( \
                (((uint16_t)(x) & (uint16_t)0x00ffU) << 8) | \
                (((uint16_t)(x) & (uint16_t)0xff00U) >> 8) ))
#define fixEndian32(x) \
        ((uint32_t)( \
                (((uint32_t)(x) & (uint32_t)0x000000ffUL) << 24) | \
                (((uint32_t)(x) & (uint32_t)0x0000ff00UL) <<  8) | \
                (((uint32_t)(x) & (uint32_t)0x00ff0000UL) >>  8) | \
                (((uint32_t)(x) & (uint32_t)0xff000000UL) >> 24) ))
#else
#define fixEndian16(x) (x)
#define fixEndian32(x) (x)
#endif

#include <netinet/in.h>
#include <common/bgp_UCI.h>

/** \brief Return the DDR size for this node in megabytes. */
static inline uint16_t BGP_Personality_DDRSizeMB(const _BGP_Personality_t *p)
{
    return p->DDR_Config.DDRSizeMB;
}

/** \brief Return the clock frequency for this node in MHz. */
static inline uint32_t BGP_Personality_clockMHz(const _BGP_Personality_t *p)
{
    return p->Kernel_Config.FreqMHz;
}

/** \brief Return the Process Config for this node. */
static inline uint32_t BGP_Personality_processConfig(const _BGP_Personality_t *p)
{
    return p->Kernel_Config.ProcessConfig;
}

/** \brief Return true (non-zero) if this node is an I/O node. */
static inline int BGP_Personality_isIONode(const _BGP_Personality_t *p)
{
    return (p->Network_Config.Xcoord == (uint8_t)-1);
}

/** \brief Return true (non-zero) if this node is a compute node. */
static inline int BGP_Personality_isComputeNode(const _BGP_Personality_t *p)
{
    return !(BGP_Personality_isIONode(p));
}

/** \brief Return the X coordinate of this node. */
static inline unsigned BGP_Personality_xCoord(const _BGP_Personality_t *p)
{
    return p->Network_Config.Xcoord;
}

/** \brief Return the Y coordinate of this node. */
static inline unsigned BGP_Personality_yCoord(const _BGP_Personality_t *p)
{
    return p->Network_Config.Ycoord;
}

/** \brief Return the Z coordinate of this node. */
static inline unsigned BGP_Personality_zCoord(const _BGP_Personality_t *p)
{
    return p->Network_Config.Zcoord;
}

/** \brief Return the X size of the block in nodes. */
static inline unsigned BGP_Personality_xSize(const _BGP_Personality_t *p)
{
    return p->Network_Config.Xnodes;
}

/** \brief Return the Y size of the block in nodes. */
static inline unsigned BGP_Personality_ySize(const _BGP_Personality_t *p)
{
    return p->Network_Config.Ynodes;
}

/** \brief Return the Z size of the block in nodes. */
static inline unsigned BGP_Personality_zSize(const _BGP_Personality_t *p)
{
    return p->Network_Config.Znodes;
}

/** \brief Return the total number of compute nodes in the block. */
static inline unsigned BGP_Personality_numComputeNodes(const _BGP_Personality_t *p)
{
    return p->Network_Config.Xnodes * p->Network_Config.Ynodes * p->Network_Config.Znodes;
}

/** \brief Return the total number of I/O nodes in the block. */
static inline uint32_t BGP_Personality_numIONodes(const _BGP_Personality_t *p)
{
    return p->Network_Config.IOnodes;
}

/** \brief Return the total number of I/O nodes in the block. */
static inline uint32_t BGP_Personality_treeNodeP2PAddrInPset(const _BGP_Personality_t *p, uint32_t psetRank)
{
    return (p->Network_Config.PSetNum * p->Network_Config.PSetSize) + psetRank;
}

static inline const char * BGP_Personality_blockName(const _BGP_Personality_t *p)
{
  return p->Ethernet_Config.NFSMountDir;   // Using the NFSMountDir to hold 
  // block name.   This field needs to be renamed. 
}

/* Max length of a location string returned by getLocation() */
#define BGPPERSONALITY_MAX_LOCATION 24

static inline void BGP_Personality_locationString(uint32_t location, char *buf)
{
  _BGP_UniversalComponentIdentifier uci;
  uci.UCI = location;
  // location string Rxx-Mx-Nxx-Jxx
  const char *hex = "0123456789ABCDEF";
  if ((uci.ComputeCard.Component == _BGP_UCI_Component_ComputeCard) ||
      (uci.IOCard.Component == _BGP_UCI_Component_IOCard)) {

    unsigned row = uci.ComputeCard.RackRow;
    unsigned col = uci.ComputeCard.RackColumn;
    unsigned mp  = uci.ComputeCard.Midplane;
    unsigned nc  = uci.ComputeCard.NodeCard;
    unsigned pc  = uci.ComputeCard.ComputeCard;

    buf[0] = 'R'; 
    buf[3] = '-'; buf[4] = 'M';
    if (row == 0xf) {
	/* generic midplane.  Literally use Rxx-Mx. */
	buf[1] = 'x'; buf[2] = 'x'; buf[5] = 'x';
    } else {
	buf[1] = hex[row]; buf[2] = hex[col]; buf[5] = hex[mp];
    }
    buf[6]  = '-'; buf[7] = 'N'; buf[8]  = hex[nc/10]; buf[9]  = hex[nc%10];
    buf[10] = '-'; buf[11] = 'J'; buf[12] = hex[pc/10]; buf[13] = hex[pc%10];
    buf[14] = '\0';
  }
  else {
    // TODO - handle other uci component types
    buf[0] = 'R'; buf[1] = 'x'; buf[2] = 'x'; buf[3] = '-'; 
    buf[4] = 'M'; buf[5] = 'x'; buf[6] = '-'; 
    buf[7] = 'N'; buf[8] = 'x'; buf[9] = 'x'; buf[10] = '-'; 
    buf[11] = 'J'; buf[12] = 'x'; buf[13] = 'x';
    buf[14] = '\0';
  }
}

static inline void BGP_Personality_getLocationString(_BGP_Personality_t* personality, char *buf)
{
    BGP_Personality_locationString(personality->Kernel_Config.UniversalComponentIdentifier, buf);
}

static inline void BGP_Personality_getIPv4Addr(_BGP_Personality_t* personality, struct in_addr* ipaddr)
{
    ipaddr->s_addr =
	(personality->Ethernet_Config.IPAddress.octet[12] << 24) +
	(personality->Ethernet_Config.IPAddress.octet[13] << 16) +
	(personality->Ethernet_Config.IPAddress.octet[14] <<  8) +
	(personality->Ethernet_Config.IPAddress.octet[15]      );
}

/** \brief true (non-zero) if block is a torus in X */
static inline int BGP_Personality_isTorusX(const _BGP_Personality_t *p)
{
    return (p->Kernel_Config.NodeConfig & _BGP_PERS_ENABLE_TorusMeshX) ? 0 : 1;
}

/** \brief true (non-zero) if block is a torus in Y */
static inline int BGP_Personality_isTorusY(const _BGP_Personality_t *p)
{
    return (p->Kernel_Config.NodeConfig & _BGP_PERS_ENABLE_TorusMeshY) ? 0 : 1;
}

/** \brief true (non-zero) if block is a torus in Z */
static inline int BGP_Personality_isTorusZ(const _BGP_Personality_t *p)
{
    return (p->Kernel_Config.NodeConfig & _BGP_PERS_ENABLE_TorusMeshZ) ? 0 : 1;
}

/** \brief return the pset number for this node */
static inline unsigned BGP_Personality_psetNum(const _BGP_Personality_t *p)
{
    return p->Network_Config.PSetNum;
}

/** \brief return the pset size */
static inline uint32_t BGP_Personality_psetSize(const _BGP_Personality_t *p)
{
    return p->Network_Config.PSetSize;
}

/** \brief return a "rank" within the pset for this node. */
static inline unsigned BGP_Personality_rankInPset(const _BGP_Personality_t *p)
{
    return p->Network_Config.RankInPSet;
}

/** \brief return the tree information for the specified tree port */

static inline unsigned BGP_Personality_getTreeInfo(const _BGP_Personality_t* p, const unsigned port) {

#if 0
    //!< \todo use this implementation when the personality is re-organized
    return (port < 3) ? p->treeInfo[port] : 0;
#endif

#if 1
    if (port==0) {
	return p->Network_Config_treeInfo0;
    }
    else if (port==1) {
	return p->Network_Config_treeInfo1;
    }
    else if (port==2) {
	return p->Network_Config_treeInfo2;
    }
    return 0; // what to do?
#endif
}

/** \brief sets the tree information for the specified tree port */

static inline unsigned BGP_Personality_setTreeInfo(_BGP_Personality_t* p, const unsigned port, const uint32_t treeInfo) {

#if 0
    if (port < 3) {
    //!< \todo use this implementation when the personality is re-organized
	p->treeInfo[port] = treeInfo;
    }
    return treeInfo;
#endif

#if 1
    if (port==0) {
	p->Network_Config_treeInfo0 = treeInfo;
    }
    else if (port==1) {
	p->Network_Config_treeInfo1 = treeInfo;
    }
    else if (port==2) {
	p->Network_Config_treeInfo2 = treeInfo;
    }
    return treeInfo;
#endif
}


/** \brief Gets the redundant attribute for the specified tree port */
static inline unsigned BGP_Personality_treeInfo_isRedundant(const _BGP_Personality_t* p, const unsigned port)
{ 
    return (BGP_Personality_getTreeInfo(p,port) & _BGP_PERS_TREEINFO_REDUNDANT) != 0; 
}

/** \brief Gets the link type attribute of the specified tree port */
static inline unsigned BGP_Personality_treeInfo_linkType(const _BGP_Personality_t* p, const unsigned port)
{ 
    return (BGP_Personality_getTreeInfo(p,port) & _BGP_PERS_TREEINFO_LINKTYPE_MASK) >> _BGP_PERS_TREEINFO_LINKTYPE_SHIFT;

}

/** \brief Gets the commWorld attribute of the specified tree port */
static inline unsigned BGP_Personality_treeInfo_commWorld(const _BGP_Personality_t* p, const unsigned port)
{
    return (BGP_Personality_getTreeInfo(p,port) & _BGP_PERS_TREEINFO_COMMWORLD_MASK) >> _BGP_PERS_TREEINFO_COMMWORLD_SHIFT; 
}

/** \brief Gets the destination port attribute for the specified tree port or -1 if the port is inactive */
static inline int BGP_Personality_treeInfo_destPort(const _BGP_Personality_t* p, const unsigned port)
{ 
    if (BGP_Personality_treeInfo_linkType(p,port) == _BGP_PERS_TREEINFO_LINKTYPE_NOWIRE) {
	return -1;
    }
    else {
	return (BGP_Personality_getTreeInfo(p,port) & _BGP_PERS_TREEINFO_DESTPORT_MASK) >> _BGP_PERS_TREEINFO_DESTPORT_SHIFT;
    }
}

/** \brief Gets the destination tree address attribute for the specified tree port */
static inline int BGP_Personality_treeInfo_destP2Paddr(const _BGP_Personality_t* p, const unsigned port)
{
    if (BGP_Personality_treeInfo_linkType(p,port) == _BGP_PERS_TREEINFO_LINKTYPE_NOWIRE) {
	return -1;
    }
    else {
	return BGP_Personality_getTreeInfo(p,port) & _BGP_PERS_TREEINFO_DESTP2P_MASK; 
    }
}


/** \brief Sets the redundant attribute for the specified tree port */
static inline unsigned BGP_Personality_treeInfo_setRedundant(_BGP_Personality_t* p, const unsigned port, const int redundant)
{ 
    uint32_t treeInfo = BGP_Personality_getTreeInfo(p,port);

    if (redundant) {
	treeInfo |= _BGP_PERS_TREEINFO_REDUNDANT;
    }
    else {
	treeInfo &= ~_BGP_PERS_TREEINFO_REDUNDANT;
    }

    return BGP_Personality_setTreeInfo(p,port,treeInfo);
}

/** \brief Sets the link type attribute for the specified tree port */
static inline unsigned BGP_Personality_treeInfo_setLinkType(_BGP_Personality_t* p, const unsigned port, const int linktype)
{ 
    uint32_t treeInfo = BGP_Personality_getTreeInfo(p,port);
    treeInfo &= ~_BGP_PERS_TREEINFO_LINKTYPE_MASK;
    treeInfo |= (linktype << _BGP_PERS_TREEINFO_LINKTYPE_SHIFT);
    return BGP_Personality_setTreeInfo(p,port,treeInfo);
}

/** \brief Sets the comm world attribute for the specified tree port */

static inline unsigned BGP_Personality_treeInfo_setCommWorld(_BGP_Personality_t* p, const unsigned port, int commWorld)
{ 
    uint32_t treeInfo = BGP_Personality_getTreeInfo(p,port);
    treeInfo &= ~_BGP_PERS_TREEINFO_COMMWORLD_MASK;
    treeInfo |= (commWorld << _BGP_PERS_TREEINFO_COMMWORLD_SHIFT); 
    return BGP_Personality_setTreeInfo(p,port,treeInfo);
}

/** \brief Sets the destination port for the specified tree port */
static inline unsigned BGP_Personality_treeInfo_setDestPort(_BGP_Personality_t* p, const unsigned port, int destinationPort)
{ 
    uint32_t treeInfo = BGP_Personality_getTreeInfo(p,port);
    treeInfo &= ~_BGP_PERS_TREEINFO_DESTPORT_MASK;
    treeInfo |=  (destinationPort << _BGP_PERS_TREEINFO_DESTPORT_SHIFT); 
    return BGP_Personality_setTreeInfo(p,port,treeInfo);
}

/** \brief Sets the destination address for the specified tree port */
static inline unsigned BGP_Personality_treeInfo_setDestP2Paddr(_BGP_Personality_t* p, const unsigned port, unsigned addr)
{ 
    uint32_t treeInfo = BGP_Personality_getTreeInfo(p,port);
    treeInfo &= ~_BGP_PERS_TREEINFO_DESTP2P_MASK;
    treeInfo |= addr;
    return BGP_Personality_setTreeInfo(p,port,treeInfo);

}

/** \brief return the tree p2p address for the I/O node for this pset.
 * The I/O node tree addresses are simply indexed by pset number starting
 * after the namespace of compute nodes.
 */
static inline unsigned BGP_Personality_treeIONodeP2PAddr(const _BGP_Personality_t *p)
{
    return BGP_Personality_numComputeNodes(p) + BGP_Personality_psetNum(p);
}

static inline unsigned get_ipv4_addr(_BGP_IP_Addr_t a)
{
        return (a.octet[12] << 24) | (a.octet[13] << 16)
          | (a.octet[14] << 8) | (a.octet[15] << 0);
}

/** \brief return the IPv4 server address for the service node.
 * This is only valid for an I/O node.  The address is in network byte order.
 */
static inline unsigned BGP_Personality_ipServiceNode(const _BGP_Personality_t *p)
{
    return get_ipv4_addr(p->Ethernet_Config.serviceNode);
}

#endif // End BGP_PERSONALITY_INLINES_H
