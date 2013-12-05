<?php

function _fms_get_lock_status($fid) {
  global $user;
  $uid = $user->uid;
  $access = eac_user_has_access($uid, 'file', $fid, 'edit');
  $checkin = db_select('fms_checkin')
    ->fields('fms_checkin')
    ->condition('fid', $fid, '=')
    ->isNull('checkout')
    ->execute()
    ->fetchAssoc();
  
//  dpm($checkin);

  if ($checkin) {
    if ($access) {
      if ($checkin['uid'] == $uid) {
        return FMS_LOCK_MYCHECKIN;
      }
      else {
        throw new Exception(t('checkin record or access is incorrect!!'));
      }
    }
    else {
      return $checkin['uid'];
    }
  }
  else if ($access) {
    return FMS_LOCK_NONE;
  }
  else {
    return FMS_LOCK_ACCESS_DENIED;
  }
}

function _fms_lock($fid) {
  global $user;
  $uid = $user->uid;
  $groups = eac_user_get_grouplist($uid, 'fms');
  $gid = $groups[0];
  return eac_access_set('file', $fid, $gid, 'edit');
}

function _fms_unlock($fid) {
  global $user;
  $uid = $user->uid;
  $groups = eac_user_get_grouplist($uid, 'fms');
  $gid = $groups[0];
  return eac_access_remove('file', $fid, $gid, 'edit');
}

function _fms_checkin($fid) {
  global $user;
  $uid = $user->uid;
  db_insert('fms_checkin')
    ->fields(array(
      'fid' => $fid,
      'uid' => $uid,
      'checkin' => time(),
      'checkout' => NULL,
    ))
    ->execute();
  _fms_lock($fid);
}

function _fms_checkout($fid) {
  global $user;
  $uid = $user->uid;
  db_update('fms_checkin')
    ->fields(array(
      'checkout' => time(),
    ))
    ->condition('fid', $fid)
    ->condition('uid', $uid)
    ->isNull('checkout')
    ->execute();
  _fms_unlock($fid);
}