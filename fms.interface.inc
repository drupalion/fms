<?php

function _fms_vdo_display_alter_form($form, &$form_state, $ctx1, $delta) {
  $fid = $ctx1['items'][$delta]['fid'];
  $lock_status = _fms_get_lock_status($fid);
  $form['fms_buttons'] = array(
    '#type' => 'fieldset',
    '#title' => t('file management system keys'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['fms_buttons']['fid'] = array(
    '#type' => 'value',
    '#value' => $fid,
  );
  $form['fms_buttons']['download'] = array(
    '#type' => 'submit',
    '#value' => t('download'),
    '#submit' => array('_fms_form_download_submit'),
  );
//  dpm($lock_status);
  switch ($lock_status) {

    case FMS_LOCK_NONE:
      $form['fms_buttons']['checkin'] = array(
        '#type' => 'submit',
        '#value' => t('check in'),
        '#submit' => array('_fms_form_checkin_submit'),
      );
      break;

    case FMS_LOCK_MYCHECKIN:
      $form['fms_buttons']['checkout'] = array(
        '#type' => 'submit',
        '#value' => t('check out'),
        '#submit' => array('_fms_form_checkout_submit'),
      );

      $form['fms_buttons']['upload'] = array(
        '#type' => 'fieldset',
        '#title' => t('replace file'),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
      );
      $form['fms_buttons']['upload']['upload_file'] = array(
        '#type' => 'managed_file',
        '#default_value' => $fid,
//        '#size' => 22,
      );
//      $form['fms_buttons']['upload']['upload_submit'] = array(
//        '#type' => 'submit',
//        '#value' => t('replace'),
//        '#submit' => array('_fms_form_file_replace_submit'),
//      );

      break;

    case FMS_LOCK_ACCESS_DENIED:
      $form['fms_buttons']['access_denied'] = array(
        '#type' => 'item',
        '#title' => t('you dont have access to edit this file'),
      );
      break;

    default :
      $user = user_load($lock_status);
      $form['fms_buttons']['checkinuser'] = array(
        '#type' => 'item',
        '#title' => t('checked in by %user', array('%user' => $user->name)),
      );
  }
  return $form;
}

function _fms_form_download_submit($form, &$form_state) {
  $fid = $form['fms_buttons']['fid']['#value'];
  $file = file_load($fid);
  $headers = array(
    'Content-Type' => 'force-download',
    'Content-Disposition' => 'attachment; filename="' . $file->filename . '"',
    'Content-Length' => $file->filesize,
    'Content-Transfer-Encoding' => 'binary',
    'Pragma' => 'no-cache',
    'Cache-Control' => 'must-revalidate, post-check=0, pre-check=0',
    'Expires' => '0',
    'Accept-Ranges' => 'bytes'
  );

  // Let other modules provide headers and controls access to the file.
  if (in_array(-1, $headers)) {
    return drupal_access_denied();
  }

  if (count($headers)) {
    file_transfer($file->uri, $headers);
  }
//  dpm($file);
//  dpm('download!');
}

function _fms_form_checkin_submit($form, &$form_state) {
  $fid = $form['fms_buttons']['fid']['#value'];
  if (_fms_get_lock_status($fid) == FMS_LOCK_NONE) {
    _fms_checkin($fid);
//    _fms_form_download_submit($form, $form_state);
  }
  else {
    throw new Exception(t('You dont have permission to checke in this file!'));
  }
}

function _fms_form_checkout_submit($form, &$form_state) {
  $fid = $form['fms_buttons']['fid']['#value'];
  if (_fms_get_lock_status($fid) == FMS_LOCK_MYCHECKIN) {
    _fms_checkout($fid);
  }
  else {
    throw new Exception(t('You dont have permission to check out this file!!'));
  }
}

function _fms_form_file_replace_submit($form, &$form_state) {
//  dargs();
  dpm($form_state,'file replace!!');
  $file = file_save_upload('upload_file',array());
  dpm($file,'file');
}